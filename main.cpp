#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "task.h"
using namespace std;
int gcd(int a, int b){
	if(a>b) swap(a,b); 
	while(b%a!=0){
		int c=b;
		b=a;
		a=c%a;
	}
	return a;
}
int lcm(int a, int b){
	return a/gcd(a,b)*b;
}
double caculateErrorCoverage(vector<task> v){
	double ans=1;
	for(int i=0;i<v.size();i++){
		if(v[i].getDetectionType()==EED){
			ans-=0.3*v[i].getExectionTime()/v[i].getPeriod();
		}
		else if(v[i].getDetectionType()==EOC){
			ans-=0;
		}
		else if(v[i].getDetectionType()==NONE){
			ans-=v[i].getExectionTime()/v[i].getPeriod();
		}
	}
	return ans;
}
void merge(list<pair<double, double> > &time){
	if(time.size()==0 || time.size()==1) return;
	auto it1 = time.begin();
	auto it2 = time.begin();
	it2++;
	while(it2!=time.end()){
		if((*it1).second==((*it2).first)){
			(*it2).first=(*it1).first;
			it1=time.erase(it1);
			it2++;
		}
		else{
			it1++;
			it2++;
		}
	}
}

bool checkWeaklyHard(task t, vector<bool> u_pattern){
	int miss=0;
	int upper = t.getConstrains().second-t.getConstrains().first;
	if(t.getHyperPeriod()>t.getConstrains().second){
		for(int i=0;i<t.getConstrains().second;i++){
			if(u_pattern[i]==0)miss++;
		}
		if(miss>upper) return false;
		for(int i=t.getConstrains().second;i<t.getHyperPeriod();i++){
			miss-=u_pattern[i-t.getConstrains().second];
			miss+=(!u_pattern[i]);
			if(miss>upper) return false;
		}
	}
	else{
		int missEveryPeriod=0;
		for(int i=0;i<t.getHyperPeriod();i++) missEveryPeriod+=(!u_pattern[i]);
		miss+=(t.getConstrains().second/t.getHyperPeriod())*missEveryPeriod;
		if(miss>upper) return false;
		upper -= miss;
		miss=0;
		int remain=t.getConstrains().second%t.getHyperPeriod();

		for(int i=0;i<remain;i++){
			if(u_pattern[i]==0)miss++;
		}
		if(miss>upper) return false;
		for(int i=remain;i<t.getHyperPeriod();i++){
			miss-=(!u_pattern[i-remain]);
			miss+=(!u_pattern[i]);
			if(miss>upper) return false;
		}
	}
	return true;
}

bool checkFeasible(vector<task> v){
	list<pair<double, double> > time;
	sort(v.begin(), v.end(), prioritySort);
	bool flag=true;
	vector<bool> u_pattern; 
	int wholePeriod=1;
	for(int i=0;i<v.size();i++){
		wholePeriod=lcm(wholePeriod, v[i].getPeriod());
	}
	int hyperperiod=1;
	for(int i=0;i<v.size();i++){
		hyperperiod=lcm(hyperperiod, v[i].getPeriod());
        u_pattern.clear();
		double execTime=v[i].getExectionTime();

		for(double j=0;j+v[i].getPeriod()<=wholePeriod;j+=v[i].getPeriod()){
			auto it = time.begin();
			double spareTime=0;
			double preEnd=j;
			while(it!=time.end()){//compute spare time before deadline
				if((*it).first>preEnd && (*it).first<=j+v[i].getDeadline()){
					spareTime+=(*it).first-preEnd;
				}
				if((*it).second>preEnd)
					preEnd=(*it).second;
				if((*it).second>=j+v[i].getDeadline())
					break;
				it++;
			}
			//cout<<(*it).first<<" "<<(*it).second<<endl;
			if(preEnd<j+v[i].getDeadline())
				spareTime+=(j+v[i].getDeadline()-preEnd);
			//cout<<spareTime<<endl;
			preEnd=j;
			if(spareTime>=v[i].getExectionTime()){
				u_pattern.push_back(1);
				double execTime=v[i].getExectionTime();
				it = time.begin();
				while(it!=time.end()){
					if((*it).first>preEnd){
						if(execTime<=(*it).first-preEnd){
							time.insert(it,pair<double, double>(preEnd, preEnd+execTime));
							execTime=0;
							break;
						}
						else{
							time.insert(it,pair<double, double>(preEnd, (*it).first));
							execTime-=(*it).first-preEnd;
						}
					}
					if((*it).second>preEnd)
						preEnd=(*it).second;
					it++;
				}
				if(execTime>0)
					time.insert(it,pair<double, double> (preEnd, preEnd+execTime));
			}
			else{
				u_pattern.push_back(0);
			}
		}
		merge(time);
		/*cout<<"u_pattern for task "<<v[i].getNo()<<":"<<endl;
		for(int k=0;k<u_pattern.size();k++)
			cout<<u_pattern[k]<<" ";
		cout<<endl;*/
		v[i].setHyperPeriod(hyperperiod/v[i].getPeriod());
		if(checkWeaklyHard(v[i],u_pattern)==false)
			flag = false;
	}
	//cout<<endl;
	auto it = time.begin();
	while(it!=time.end()){
		//cout<<(*it).first<<" "<<(*it).second<<endl;
		it++;
	}
	return flag;
}
void initialPriority(vector<task> &s){
	sort(s.begin(),s.end(),deadlineSort);
	for(int i=0;i<s.size();i++){
		s[i].setPriority(i+1);
	}
}

vector<task> simulateAnneal(vector<task> &s, double &errorCoverage){
	errorCoverage=0;
	vector<task> s1;
	int times=10000;
	double T=100,r=0.5;
	while(times--){
		int rand1=rand()%s.size();
		int rand2=rand()%s.size();
		int rand3=rand()%s.size();
		int rand4=rand()%3;
		s[rand3].setDetectionType(rand4);
		int t=s[rand1].getPriority();
		s[rand1].setPriority(s[rand2].getPriority());
		s[rand2].setPriority(t);
		if(checkFeasible(s)==true){
			double nowErrorCoverage=caculateErrorCoverage(s);
			double de=nowErrorCoverage-errorCoverage;
			if(de>0){
				errorCoverage=nowErrorCoverage;
				s1.clear();
				for(int i=0;i<s.size();i++)
					s1.push_back(s[i]);
				T=r*T;
				continue;
			}
			else{
				if(exp(de/T)>(((double)rand()/(RAND_MAX))+1)){
					T=r*T;
					continue;
				}
			}
		}
		T=r*T;
		t=s[rand1].getPriority();
		s[rand1].setPriority(s[rand2].getPriority());
		s[rand2].setPriority(t);
	}
	return s1;
}

vector<task> initialTasks(){
	srand (time(NULL));
	vector<task> s(4);
	pair<int, int> p(1,1);
	s[0]=task(0,0.2,1,1,p);
	s[1]=task(1,0.45,2,2,p);
	s[2]=task(2,0.1,2,2,p);
	s[3]=task(3,0.1,2,2,p);
	initialPriority(s);
	return s;
}

int main(){
	vector<task> s=initialTasks();
	//s[2]=task(2,1,5,5,p);
	double errorCoverage;
	vector<task> t = simulateAnneal(s, errorCoverage);
	cout<<"Error Coverage is:"<<errorCoverage<<endl;
	for(int i=0;i<t.size();i++){
		cout<<"No:"<<t[i].getNo()<<" Priority:"<<t[i].getPriority()<<" Security Method:";
		if(t[i].getDetectionType()==NONE)
			cout<<"NONE"<<endl;
		else if(t[i].getDetectionType()==EED)
			cout<<"EED"<<endl;
		else if(t[i].getDetectionType()==EOC)
			cout<<"EOC"<<endl;
	}
}
