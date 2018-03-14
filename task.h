#ifndef _TASK_H_
#define _TASK_H_
#include <iostream>
#include <utility>
using namespace std;
enum errorToleranceType{NONE,EED,EOC};
class task{
private:
  double deadline;
  double period;
  double exectionTime;
  int priority;
  int detectionType;
  pair <int, int> constrains;
  int no;
  int hyperPeriod;
  double normalTime;
public:
  task(){
  }
  task(int n,double a, double b, double c, pair <int, int> d):no(n), normalTime(a), period(b), deadline(c), constrains(d){
    detectionType=NONE;
    exectionTime=normalTime;
  }
  void setHyperPeriod(int hyper){
    hyperPeriod=hyper;
  }
  int getHyperPeriod(){
    return hyperPeriod;
  }
  int getNo(){
    return no;
  }
  int getDetectionType(){
    return detectionType;
  }
  double getDeadline(){
    return deadline;
  }
  double getPeriod(){
    return period;
  }
  double getExectionTime(){
    return exectionTime;
  }
  int getPriority(){
    return priority;
  }
  void setDetectionType(int a){
    detectionType=a;
    if(a==NONE)
      exectionTime=normalTime;
    else if(a==EED)
      exectionTime=normalTime*1.5;
    else if(a==EOC)
      exectionTime=normalTime*2;
  }
  void setPriority(int p){
    priority=p;
  }
  pair<int, int> getConstrains(){
    return constrains;
  }
  bool operator >(task a){
    return priority > a.priority;
  } 
  bool operator <(task a){
    return priority < a.priority;
  }
};
bool deadlineSort(task a, task b){
  return a.getDeadline()<b.getDeadline(); 
}
bool prioritySort(task a, task b){
  return a.getPriority()<b.getPriority();
}
#endif
