#include "ncaa_test.h"

Tester::Tester()
{
	printf("Creating network.\n");
	ann = fann_create_from_file("neuralNetwork/NCAA_ANN_src/ncaa_float.net");
	accuracy = 0.0;
	parse_teams("neuralNetwork/NCAA_ANN_src/teams.txt");
}

//construcor for automated testing purposes only
Tester::Tester(int test)
{
	printf("Automated Testing.\n");
	ann = fann_create_from_file("../../ncaa_float.net");	
	accuracy = 0.0;
}

//returns the name of the winner after running the algorithm on the input teams
string Tester::getPrediction(string team1,string team2)
{
	fann_type *calc_out;
	if(!ann)
	{
		printf("Error creating ann --- ABORTING.\n");
		return "Error with network\n";
	}

	fann_print_connections(ann);
	fann_print_parameters(ann);

	printf("Testing network.\n");
	struct fann_train_data *data = fann_read_train_from_file("ncaa.data");
	map<string, vector<float> >::const_iterator it_team1 = team_stats.find(team1);	
	if(it_team1 != team_stats.end())
	{
		map<string, vector<float> >::const_iterator it_team2 = team_stats.find(team2);
		if(it_team2 != team_stats.end())
		{
			if(it_team1->first != it_team2->first)
			{
				fann_type stat_input[(it_team1->second).size() + (it_team2->second).size()];
				int j = 0;
				for(j = 0; j<(it_team1->second).size(); j++)
				{
					stat_input[j] = ((it_team1->second)[j]);
				}
				for(int k = 0; k<(it_team2->second).size(); k++)
				{
					stat_input[j + k] = ((it_team2->second)[k]);
				}
				
				fann_type * const calc_team_score = run_fann(ann, stat_input);				

				if(calc_team_score[0] < 0.5)
				{	
					//team 2 won
					
					printf("Cleaning up.\n");
					fann_destroy_train(data);

					return team2;	
				}
				else
				{	
					//team 1 won
					
					printf("Cleaning up.\n");
					fann_destroy_train(data);

					return team1;	
				}
			}
		}
		else
		{
			printf("Cleaning up.\n");
			fann_destroy_train(data);

			return "Both teams are same.\n";
		}
	}
	else
	{	
		printf("Cleaning up.\n");
		fann_destroy_train(data);
		
		return "One or both team names don't exist. Sorry!\n";
	}

}

//runs the algorithm using the network and the fann_type input
fann_type * const Tester::run_fann(struct fann * ann, fann_type stat_input[])
{
	return fann_run(ann, stat_input);			
}

//parses the teams stats and initializes maps
void Tester::parse_teams(char * fileName)
{
	FILE *teams_file = fopen(fileName, "r");
        if (teams_file == NULL)
                perror("Teams file is empty");

        char * line = NULL;
        size_t len = 0;
        size_t read;
	char * pch;
       	int pch_flag = 0;
	string team_name;
	while ((read = getline(&line, &len, teams_file)) != -1)
        {
	
		if(pch_flag == 0)
		{
			team_name  = string(line, strlen(line)-1);
			pch_flag = 1;
		}
		else
		{
			vector<float> avg_stats;
			pch = strtok(line, " ");
			while(pch != NULL)
                	{
				float stat = 0.0;
                                istringstream (pch) >> stat;
				avg_stats.push_back(stat);	
                        
                        	pch = strtok(NULL, " ");
			}
			pch_flag = 0;	
               		team_rank[team_name] = 0;
			team_stats[team_name] = avg_stats;
		}
		
	}
}

//prints the calculated ranking of teams with their score
void Tester::print_rank()
{
	map<int, vector<string> > result_map;	

	//sorts by value instead of keys in descending order
	map<string, int>::const_iterator it;
	for(it = team_rank.begin(); it != team_rank.end(); ++it)
	{
		map<int, vector<string> >::const_iterator search = result_map.find((-1)*it->second);
		if(search == result_map.end())
		{
			vector<string> s;
			s.push_back(it->first);
			result_map[(-1)*it->second] = s;
		}
		else
		{
			vector<string> old_s = search->second;
			old_s.push_back(it->first);
			result_map[search->first] = old_s;
		}
	}

	int i = 0;
	map<int, vector<string> >::const_iterator result_it;
	printf("RANKING:\n");
	for(result_it = result_map.begin(); result_it != result_map.end(); ++result_it)
	{
		vector<string>::const_iterator it_vec;
                for(it_vec = (result_it->second).begin(); it_vec != (result_it->second).end(); ++it_vec)
                {
     			i++;
			printf("%d. %s => %d\n", i, (*it_vec).c_str(), (-1)*result_it->first);
		}
	}
}

//calculates rank by running algorithm on each possible matchup and adding 1 for win or subtracting 1 for loss
void Tester::calc_rank()
{
	map<string, vector<float> >::const_iterator it_team1;	
	for(it_team1 = team_stats.begin(); it_team1 != team_stats.end(); ++it_team1)
	{
		map<string, vector<float> >::const_iterator it_team2;
		for(it_team2 = team_stats.begin(); it_team2 != team_stats.end(); ++it_team2)
		{
			if(it_team1->first != it_team2->first)
			{
				fann_type stat_input[(it_team1->second).size() + (it_team2->second).size()];
				int j = 0;
				for(j = 0; j<(it_team1->second).size(); j++)
				{
					stat_input[j] = ((it_team1->second)[j]);
				}
				for(int k = 0; k<(it_team2->second).size(); k++)
				{
					stat_input[j + k] = ((it_team2->second)[k]);
				}
				
				fann_type * const calc_team_score = run_fann(ann, stat_input);				

				if(calc_team_score[0] < 0.5)
				{	
					//team 2 won
					map<string, int>::const_iterator team2_search = team_rank.find(it_team2->first);
					if(team2_search != team_rank.end())
					{
						team_rank[it_team2->first] = (team2_search->second) + 1;
					}
					
					//team 1 lost
					map<string, int>::const_iterator team1_search = team_rank.find(it_team1->first);
					if(team1_search != team_rank.end())
					{
						team_rank[it_team1->first] = (team1_search->second) - 1;
					}
				}
				else
				{	
					//team 2 lost
					map<string, int>::const_iterator team2_search = team_rank.find(it_team2->first);
					if(team2_search != team_rank.end())
					{
						team_rank[it_team2->first] = (team2_search->second) - 1;
					}

					//team 1 won
					map<string, int>::const_iterator team1_search = team_rank.find(it_team1->first);
					if(team1_search != team_rank.end())
					{
						team_rank[it_team1->first] = (team1_search->second) + 1;
					}
				}
			}
		}
	}

	print_rank();
}

//returns the accuracy
float Tester::get_accuracy()
{
	return accuracy;
}

//tests the netowork and calculates accuracy
int Tester::test_network()
{
	fann_type *calc_out;
	unsigned int i;
	int ret = 0;
	int num_test_passed = 0;
	int num_total_tests = 0;
	
	if(!ann)
	{
		printf("Error creating ann --- ABORTING.\n");
		return -1;
	}

	fann_print_connections(ann);
	fann_print_parameters(ann);
	
	printf("Testing network.\n");
	struct fann_train_data *data = fann_read_train_from_file("neuralNetwork/NCAA_ANN_src/ncaaTest.data");
		
	for(i = 0; i < fann_length_train_data(data); i++)
	{
		fann_reset_MSE(ann);
		calc_out = fann_test(ann, data->input[i], data->output[i]);

		//printf("Matchup scores test (%f, %f) -> %f, should be %f, difference=%f\n",
		//	   data->input[i][0], data->input[i][1], calc_out[0], data->output[i][0],
		//	   (float) fann_abs(calc_out[0] - data->output[i][0]));
		num_total_tests++;
		if(((float) fann_abs(calc_out[0] - data->output[i][0])) < 0.5)
			num_test_passed++;
	}

	accuracy = ((float)num_test_passed) / ((float) num_total_tests);
	printf("Accuracy = %f percent\n", accuracy*100.00);
	calc_rank();
	fann_destroy_train(data);

	return ret;
}

Tester::~Tester()
{
	printf("Cleaning up.\n");
	fann_destroy(ann);
}
