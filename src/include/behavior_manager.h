/*!********************************************************************************
 * \brief     behavior_manger definition file
 * \authors   Pablo Santamaria, Martin Molina
 * \copyright Copyright (c) 2020 Universidad Politecnica de Madrid
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef BEHAVIOR_MANAGER_H
#define BEHAVIOR_MANAGER_H

#include <string>
#include <list>
#include <algorithm>
#include <thread>

// ROS
#include <ros/ros.h>
#include <ros/this_node.h>
#include <ros/master.h>

// Aerostack
#include "../include/task.h"
#include "../include/constraint.h"
#include "../include/behavior.h"
#include "../include/catalog.h"
#include <aerostack_msgs/StartTask.h>
#include <aerostack_msgs/StopTask.h>
#include <aerostack_msgs/TaskStopped.h>
#include <aerostack_msgs/ListOfRunningTasks.h>
#include <aerostack_msgs/TaskCommand.h>
#include <aerostack_msgs/BehaviorActivationFinished.h>
#include <aerostack_msgs/ActivateBehavior.h>
#include <aerostack_msgs/DeactivateBehavior.h>

class BehaviorManager
{
public:
  BehaviorManager();
  ~BehaviorManager();

  int DEFAULT_ACTIVATION_DELAY = 80; //ms
  int numberOfNoGoodConstraints = 0;
  int MAXIMUM_PERFORMANCE = 100;
  int MAXIMUM_NO_GOOD_CONSTRAINTS = 100;
  int MAXIMUM_SEARCH_TIME = 2000; //ms

  Catalog* catalog;
  bool first_call = true;
  std::list<std::pair<Task, std::list<Behavior*>>> bestBehaviorAssignment;
  std::list<std::pair<Task*, std::chrono::time_point<std::chrono::system_clock>>> defaultTasksToActivate;
  std::chrono::time_point<std::chrono::system_clock> startSearchTime, endSearchTime;
  std::list<std::pair<Task*,std::list<Constraint*>>> tasksConstraints;
  std::list<std::pair<Task, std::list<Behavior*>>> resetActivationList;
  std::list<Behavior*> lastAssignment;
  
  std::string robot_id;
  std::string catalog_path;
  std::string robot_namespace;
  std::string behavior_activation_finished_str;
  std::string start_task_str;
  std::string stop_task_str;
  std::string task_stopped_str;
  std::string consult_available_behaviors_str;
  //std::string list_of_running_tasks_str;
  std::string activation_change_str;
  ros::ServiceClient activate_behavior_srv;
  ros::ServiceClient deactivate_behavior_srv;
  ros::ServiceServer start_task_srv;
  ros::ServiceServer stop_task_srv;
  ros::Subscriber behavior_activation_finished_sub;
  ros::Publisher task_stopped_pub;
  //ros::Publisher list_of_running_tasks_pub;
  ros::Publisher activation_change_pub;

  void behaviorActivationFinishedCallback(const aerostack_msgs::BehaviorActivationFinished &message);
  bool activateTaskCallback(aerostack_msgs::StartTask::Request &request, aerostack_msgs::StartTask::Response &response);
  bool deactivateTaskCallback(aerostack_msgs::StopTask::Request &request, aerostack_msgs::StopTask::Response &response);

  // DroneProcess
  void init();
  void resetDomains();

  //DRIVE
  bool propagateConstraints();
  bool generateAssignment();
  bool testPerformanceConstraints();
  bool generateConsistentAssignment();
  Behavior* selectValueToExplore(Task* tasksit, std::list<Behavior*> searchDomain);
  bool checkAssignmentFound();
  std::list<std::list<Behavior*>> retrieveDomains();
  void restoreDomains(std::list<std::list<Behavior*>> savedDomains);
  bool generateBestAssignment(int priority, std::pair<Task*, std::list<Behavior*>> desiredDomain);
  void initializeSearch(int priority, std::pair<Task*, std::list<Behavior*>> desiredDomain);
  void removeNoGoodConstraints();
  void initializeSearchTime();
  void calculateTasksPerformance();
  void calculatePerformance();
  void retrieveAssignment(std::list<std::pair<Task, std::list<Behavior*>>>& assignment);
  bool addNoGood(std::list<std::pair<Task, std::list<Behavior*>>> assignment);
  int evaluateGlobalPerformance();
  bool exceededMaximumSearchTime();
  bool generateAndExecuteAssignment(int priority, std::pair<Task*, std::list<Behavior*>> desiredDomain);
  bool executeAssignment();
  void resetActivations();
  void setUp();
  std::list<Behavior**> calculateTaskDependencies(Task* taskIterator);
  Behavior** getMaxEfficacyValue(Task* taskIterator);
  bool checkConstraint(Constraint* constraint);
  std::list<std::pair<Task, std::list<Behavior*>>> generateAssignmentExplainingViolation(Constraint* constraint);
  double evaluateObjectiveFunction1MinActive();
  double evaluateObjectiveFunction1MinChanges();
  double evaluateObjectiveFunction1MaxEfficacy();
  double evaluateObjectiveFunction2();
  void loadTasksConstraints();
  bool activateBehavior(Behavior* behavior, std::string arguments, int priority);
  bool deactivateBehavior(Behavior behavior);
  void deactivateBehaviorFailed(Behavior behavior);
  void stopBehaviorManager();
  void initializeIncompatibleTasks();
  void checkDefaultActivations();
  void checkTimeouts();
  void publishListOfRunningTasks();

  //DEBUG
  void printBestBehaviorAssignment();
  int solutions = 0;
  std::list<Constraint*> agendaG;
  std::list<Constraint*> noGoodconstraints;
  std::list<Constraint*> arcs;
  bool testing;

private:

  bool DEBUG;

  //Communication variables
  ros::NodeHandle node_handle;
  ros::Publisher execution_request_pub;
  std::string execution_request_str;
};

#endif