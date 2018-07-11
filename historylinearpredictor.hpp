#include<Eigen/Dense>
#include<Eigen/Cholesky>
#include<vector>
#include<algorithm>


//http://n.schraudolph.org/pubs/ChiTruSch08.pdf
//https://en.wikipedia.org/wiki/Vector_autoregression
//https://en.wikipedia.org/wiki/Levinson_recursion
//https://en.wikipedia.org/wiki/Autoregressive_model

class CircularHistoryBase
{
protected:
	void history_append(const double* input,size_t num_to_append)
	{
		double* hqb=history_queue.data();
		num_to_append = num_to_append > num_history ? num_history : num_to_append;
		std::rotate(hqb,hqb+(num_history-num_to_append)*num_inputs,hqb+num_history*num_inputs);
		std::copy(input,input+num_to_append*num_inputs,hqb);
	}
	
	Eigen::VectorXd history_queue;
public:
	size_t num_history;
	size_t num_outputs;
	size_t num_inputs;
	
	CircularHistoryBase(size_t nh,size_t no,size_t ni):
		num_history(nh),
		num_outputs(no),
		num_inputs(ni),
		history_queue(Eigen::VectorXd::Zero(nh*ni))
	{}
};


class HistoryLinearPredictor: public CircularHistoryBase
{
public:
	Eigen::MatrixXd coefficients;	//should this design make the coeffs transposed?  Yes.
	HistoryLinearPredictor(size_t nh,size_t no,size_t ni,const Eigen::MatrixXd& c):
		CircularHistoryBase(nh,no,ni),
		coefficients(c)
	{}
	void update(const double* input,size_t num_inputs_array=1)
	{
		history_append(input,num_inputs_array);
	}
	Eigen::VectorXd predict() const
	{
		return coefficients*history_queue;
	}
	void restart()
	{
		memset(history_queue.data(),0,sizeof(double)*history_queue.rows());
	}
};

class HistoryLinearModel: public CircularHistoryBase
{
private:
	Eigen::LDLT<Eigen::MatrixXd> ldl;
	Eigen::MatrixXd AtA;
public:
	Eigen::MatrixXd lefttracker;
	
	HistoryLinearModel(size_t nh,size_t no,size_t ni):
		CircularHistoryBase(nh,no,ni),
		ldl(nh*ni),
		lefttracker(Eigen::MatrixXd::Zero(ni*nh,no))
	{
		ldl.setZero();
		AtA=Eigen::MatrixXd::Zero(history_queue.rows(),history_queue.rows());
	}
	//potentially erase from the end of the queue if history length is limited...
	void train(const double* predicted,const double* current)  //
	{
		history_append(current,1);
		Eigen::Map<const Eigen::MatrixXd> outpm(predicted,1,num_outputs);
		
		lefttracker+=history_queue*outpm;
		AtA+=history_queue*history_queue.transpose();
		//ldl.rankUpdate(history_queue);
		
		//
		
		//(A^T A)^-1 A^t*outpm=input;
	}
	
	HistoryLinearPredictor predictor() const
	{
		//return HistoryLinearPredictor(num_history,num_outputs,num_inputs,ldl.solve(lefttracker).transpose());
		return HistoryLinearPredictor(num_history,num_outputs,num_inputs,AtA.ldlt().solve(lefttracker).transpose());
	}
};

class EquispacedHistoryLinearModel: public HistoryLinearModel
{
protected:
	std::vector<double> equispace_queue;
	size_t current_queue_depth;
public:
	unsigned int steplength;
	
	EquispacedHistoryLinearModel(size_t nh,size_t no,size_t ni,unsigned int sl=1):
		HistoryLinearModel(nh,no,ni),
		steplength(sl),
		current_queue_depth(0),
		equispace_queue(ni*nh)
	{}
	void train(const double* predicted)
	{
		
	}
};
