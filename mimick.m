function [out]=mimick(y,N)
%given y samples (each one is a real number) from an arbitrary continuous distribution, 
%generate N random points mimicking the same continuous distribution 
	y=y(:);
	n=length(y);
	ys=sort(y);
	x=[0:(1/(n-1)):1]';
	xq=rand(1,N);
	out=interp1(x,ys,xq,'spline');

	%this should be a lagrange interpolating polynomial?  or a spline or cubic? Make sure it's a linear time one
end


% >> mimick([5 5 1 5 1 1 5 5 5 5 1],10)
% >> mimick([4.1 4.2 4.3 3.9 3.8 4.4],100)
% >> mimick([0 1 2 3 4 1 3 2 1 3 4 0],14)
% >> mimick([4.1 8.3 4.2 4.3 3.9 7.8 3.8 4.4 8.1 8.0 7.9],100) %mimick learns a two-peaked uniform distribution
% >> y=mimick(randn(400,1),1000); std(y) %mimick learns to create a gaussian distribution with var=1
