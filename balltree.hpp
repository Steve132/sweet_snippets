template<unsigned int D>
class balltree
{
public:
	struct ball
	{
		Eigen::Matrix<double,D,1> position;
		double radius;
		size_t leaf_id;
	};
	ball boundary;
	std::shared_ptr<balltree> left;
	std::shared_ptr<balltree> right;
	//sum (x-mx)*(y-my)' = sum x*y - mx * sum y - my * sum x - sum mx*my;
	//sum x*y - mx * sum y - my * sum x - sum mx*my;
	//sum x*y - 2*outer product of sums / N  + outer product of sums
	
	balltree(ball* bbegin,ball* bend)
	{
		Eigen::Matrix<double,D,1> pos_sum;
		Eigen::Matrix<double,D,D> pos_lcov;
		size_t n=bend-bbegin;
		if(n==1)
		{
			boundary=*bbegin;
			return;
		}
		
		Eigen::Matrix<double,D,1> pos_mean,childaxis;
		
		if(n==2)
		{
			childaxis=bbegin[1].position-bbegin[0].position;
			pos_mean=bbegin[0].position+childaxis*0.5;
			childaxis.normalize();
			left=make_shared<balltree>(bbegin,bbegin+1);
			right=make_shared<balltree>(bbegin+1,bbegin+2);
		}
		else
		{
			for(const ball* bi=bbegin;bi != bend;bi++)
			{
				pos_sum+=bi->position;
				pos_lcov+=epos*epos.transposed();
			}
			double nf=static_cast<double>(n);
			pos_mean=pos_sum/nf;
			Eigen::Matrix<double,D,D> outer_prod_sums=pos_sum*pos_sum.transposed();
			Eigen::Matrix<double,D,D> pos_lcov=pos_lcov-outer_prod_sums*(2.0/nf+1.0/(nf*nf));
			childaxis=//evd(pos_lcov).largest_vector();
			
			//std::partition(bbegin,b  mean based splitting.
			
			std::nth_element(bbegin,bbegin+n/2,bend,
				[childaxis,pos_mean](const ball& a,const ball& b)
				{
					return (a.position-pos_mean).dot(childaxis) < (b.position-pos_mean).dot(childaxis);
				}
			);
			left=make_shared<balltree>(bbegin,bbegin+n/2);
			right=make_shared<balltree>(bbegin+n/2,bend);
		}
		boundary.leaf_id=0;
		boundary.leaf_id=~boundary.leaf_id;
		Eigen::Vector<double,D,1> lower=left->boundary.position-left->boundary.radius*childaxis;
		Eigen::Vector<double,D,1> upper=right->boundary.position+right->boundary.radius*childaxis;
		boundary.radius=(upper-lower).mag()/2.0;
		boundary.position=(upper+lower)/2.0;
	}
};
