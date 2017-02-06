
class CircularHistoryBase
{
protected:
	void history_append(const double* input,size_t num_to_append)
	{
		double* hqb=history_queue.data();
		double* tgt=hqb+(num_history-1)*num_inputs;
		double* src=tgt-1;
		num_to_append=num_to_append > num_history ? num_history : num_to_append;
		num_history*=num_inputs;
		num_to_append*=num_inputs;
		for(size_t i=num_to_append;i<num_history;i++)
		{
			*(tgt--)=*(src--);
		}
		std::copy(input,input+num_to_append,hqb);
	}
	
	std::VectorXd history_queue;
public:
	size_t num_history;
	size_t num_outputs;
	size_t num_inputs;
	
	HistoryLinearPredictor(size_t nh,size_t no,size_t ni):
		num_history(nh),
		num_outputs(no),
		num_inputs(ni),
		history_queue(nh*ni,1),
	{}
};

class HistoryLinearModel: public CircularHistoryBase
{
private:
	LDL<Eigen::MatrixXd,double> ldl;
	
public:
	Eigen::MatrixXd lefttracker;
	
	HistoryLinearModel(size_t nh,size_t no,size_t ni):
		CircularHistoryBase(nh,no,ni),
		ldl(nh*ni),
		lefttracker(MatrixXd::Zero(ni*nh,no))
	{
		ldl.setZero();
	}
	//potentially erase from the end of the queue if history length is limited...
	void train(const double* output,const double* input)
	{
		history_append(input,1);
		Eigen::Map<MatrixXd> outpm(output,1,num_outputs);
		
		lefttracker+=history_queue*outpm;
		ldl.rankUpdate(history_queue);
	}
	HistoryLinearPredictor predictor() const
	{
		return HistoryLinearPredictor(num_history,num_outputs,num_inputs,ldl.solve(lefttracker).transposed());
	}
};

class HistoryLinearPredictor: public CircularHistoryBase
{
public:
	MatrixXd coefficients;	//should this design make the coeffs transposed?  Yes.
	HistoryLinearPredictor(size_t nh,size_t no,size_t ni,const MatrixXd& c=MatrixXd(no,ni*nh))
		CircularHistoryBase(nh,no,ni)
	{}
	void update(const double* input,size_t num_inputs_array=1)
	{
		history_append(input,num_inputs_array);
	}
	VectorXd predict() const
	{
		return coefficients*history_queue;
	}
};
