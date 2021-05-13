/*
 * admin.h
 *
 *  Created on: 06-May-2021
 *
 *  Includes the Admin class to provide the functionalities required by an Administrator.
 */

#ifndef ADMIN_H_
#define ADMIN_H_

#include <string>
#include <ctime>
#include "citizen.h"
#include "dateutil.h"
using namespace std;

class Admin
{
	string pswd;
	static int available_shots; // The number of vaccine shots available per day
	static string last_date;	// Used by the run_process function of this class to record the date of its last execution

public:
	static int age_eligibility; // The minimum age eligible for registration

	Admin()
	{
		/*
		 * Constructor
		 */
		pswd = "vaccine";
	}
	bool run_process(CitizenQueueList &cqueue);
	void display_top_n(CitizenQueueList &cqueue);
	void display_today(CitizenQueueList &cqueue);
	void change_eligibility(CitizenQueueList &cqueue);
	void change_available_shots();
	bool verify_password(string pass);
	void remove(Citizen *user, string pswd, CitizenQueueList &cqueue);
	void remove_age_ineligible(CitizenQueueList &cqueue);

	friend class CitizenQueueList;
};
// ======================================================================================================
// Admin class implementation

// Static member initializations
int Admin::age_eligibility = 18;
int Admin::available_shots = 3;
string Admin::last_date = "";

bool Admin::verify_password(string pass)
{
	/*
	 * Verifies the Admin password.
	 * Returns true if the password matches.
	 */
	return pass == pswd;
}

bool Admin::run_process(CitizenQueueList &cqueue)
{
	/*
	 * The most important functionality of this class.
	 * It allocates slots to citizens in a priority queue-list.
	 * Returns true if the allocation process has executed.
	 */

	Citizen *selected = cqueue.front;
	Citizen *dq = nullptr;

	// Extracting today's date
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int day = ltm->tm_mday;
	int month = ltm->tm_mon + 1;
	int year = ltm->tm_year + 1900;
	string today = date_string(day, month, year); // The representation of a today's date as a string

	if (today == last_date) // This prevents multiple allotment rounds on the same day.
		return false;

	// ALLOTMENT PROCESS:

	// For citizens who have been allocated slots previously
	while (selected != nullptr && selected->visible)
	{
		if (selected->slot_date == today) // Citizens whose slot is today
			selected->slot_today = true;  // Modifies the citizen's slot_today member
		else
		{
			// Citizens whose slot occurred yesterday, but who failed to get vaccinated,
			// need to be removed to the end of the queue-list (demoted) so they can be assigned fresh slots.
			int new_priority = cqueue.get_min_priority() - 1;
			dq = cqueue.dequeue();
			dq->visible = false;
			dq->slot_today = false;
			dq->demoted = true;
			dq->priority = new_priority;

			cqueue.enqueue(dq);
		}
		selected = selected->next;
	}
	string dslot = get_next_date(day, month, year); // Generates the string representation for the following day.

	// Allocates the following day's slot to the citizens and enables them to check their date slots.
	// Only a maximum of 'available_shots' number of citizens can be allocated a slot on a given day.
	for (int count = 1; count <= available_shots; count++)
	{
		if (selected == nullptr)
			break;
		selected->slot_date = dslot;
		selected->visible = true;
		selected = selected->next;
	}

	last_date = today; // Modifies the last_date member to today's date

	return true;
}

void Admin::display_top_n(CitizenQueueList &cqueue)
{
	unsigned n;
	cout << "\n\tHow many entries are to be displayed ?";
	cin >> n;
	cqueue.display_top_n(n);
}

void Admin::display_today(CitizenQueueList &cqueue)
{
	cqueue.display_today();
}
void Admin::change_eligibility(CitizenQueueList &cqueue)
{
	/*
	 * Allows admin to change the minimum eligibility age
	 */
	cout << "\n\tEnter eligibility age : ";
	cin >> age_eligibility;
	remove_age_ineligible(cqueue);
	cout << "\n\t\tChanged successfully." << endl;
}
void Admin::change_available_shots()
{
	/*
	 * Allows admin to change available_shots as per actual availability
	 */
	cout << "\n\tEnter new number : ";
	cin >> available_shots;
	cout << "Changed successfully." << endl;
}

void Admin::remove(Citizen *user, string pswd, CitizenQueueList &cqueue)
{
	cqueue.remove(user, pswd);
}

void Admin::remove_age_ineligible(CitizenQueueList &cqueue)
{
	Citizen *cur = cqueue.front;
	Citizen *prev = nullptr;
	Citizen *to_delete = nullptr;
	while (cur != NULL)
	{
		/*
		* find citizens with age below age_eligibility and 
		* those who havent been alloted a time slot
		*/
		if (cur->age < age_eligibility && !cur->visible)
		{
			if (prev == NULL) //first user to be deleted
			{
				cqueue.front = cur->next;
				to_delete = cur;
				cur = cur->next;
				to_delete->next == NULL;
				delete (to_delete);
			}
			else if (cur == cqueue.rear) //last user to be deleted
			{
				cqueue.rear = prev;
				prev->next = NULL;
				to_delete = cur;
				cur = prev;
				delete (to_delete);
			}
			else //in-between user to be deleted
			{
				prev->next = cur->next;
				to_delete = cur;
				cur = cur->next;
				to_delete->next = NULL;
				delete (cur);
			}
		}
		prev = cur;
		cur = cur->next;
	}
}
// ======================================================================================================

#endif /* ADMIN_H_ */
