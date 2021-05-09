#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "covid.h"


void MovePerson(int personID, Station* station1, Station* station2, PtrtoPerson *person_list)
{
    // Deleting Person from station1
    separateHash(station1->PeopleList, personID, person_list[personID-1]->type, '-');
    if (person_list[personID-1]->type == covid_positive)
        station1->danger_value--;
    if (person_list[personID-1]->type == primary_contact)
        station1->danger_value -= 1/5;
    if (person_list[personID-1]->type == secondary_contact)
        station1->danger_value -= 1/10;
    station1->no_of_people--;

    //Adding person to station2
    PtrtoPerson person = separateHash(station2->PeopleList, personID, person_list[personID-1]->type, '+');
    double danger_value = Update(station2, person, person_list);
    station2->no_of_people++;
    station2->danger_value = danger_value;
    
}

double Update(Station *S, PtrtoPerson P, PtrtoPerson *person_list)
{
    ll covid = 0, prim = 0, second = 0;

    // just in case P is safe and goes to place with primary so update P
    for (ll i = 0; i < S->PeopleList->TableSize; i++)
    {
        PtrtoPerson Current = S->PeopleList->pStart[i]->next;
        while (Current != NULL)
        {
            if (Current->type < P->type){
                Key k = Identityhash(P->ID, S->PeopleList->TableSize);
                PtrtoPerson Position = S->PeopleList->pStart[k];
                while ((Position->next != NULL) && (Position->next->ID != P->ID)){
                    Position = Position->next;
                }
                if (Position->next->ID == P->ID){
                    Position->next->type = Current->type + 1;
                    person_list[P->ID-1]->type = Current->type + 1;
                }
                goto L1;
            }

            if (Current->type - P->type > 1){
                Current->type = P->type + 1;
                person_list[Current->ID - 1]->type = P->type + 1;
            }

            if (Current->type == P->type && Current->ID != P->ID)
                goto L1;

            Current = Current->next;
        }
    }

    L1:

    // Finding the number of covid, primary and secondary people;
    for (int i = 0; i < S->PeopleList->TableSize; i++)
    {
        PtrtoPerson Current = S->PeopleList->pStart[i]->next;
        while (Current != NULL)
        {
            if (Current->type == 0)
                covid++;
            if (Current->type == 1)
                prim++;
            if (Current->type == 2)
                second++;
            Current = Current->next;
        }
    }
    
    double danger_value = (double)covid + ((double)prim / 5) + ((double)second / 10);

    return danger_value;
}

