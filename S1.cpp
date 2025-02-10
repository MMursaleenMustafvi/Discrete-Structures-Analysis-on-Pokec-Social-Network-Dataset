#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <queue>
#include <windows.h>
#include<vector>
using namespace std;
struct Profile
{
	int id;
	int publicstatus;
	int age;
	int loggedin;
	string region;
	string last_login;
	string joined_date;
	int complication;

	void display() const
	{
		cout << "*====================================*" << endl;
		cout << "|ID:" << id << endl;
		cout << "|AGE:" << age << endl;
		cout << "|PUBLIC STATUS: " << publicstatus << endl;
		cout << "|LOGGED IN:" << loggedin << endl;
		cout << "|REGION:" << region << endl;
		cout << "|LAST LOGIN:" << last_login << endl;
		cout << "|JOINED:" << joined_date << endl;
		cout << "|COMPLICATION:" << complication << "%" << endl;
		cout << "*====================================*" << endl;
		cout << endl;
	}
};
const int maximum_profile = 1000;
const int maximum_relation = 1000;
int r_count = 0;
Profile profiles[maximum_profile];
int relationships[maximum_relation][maximum_relation]; 
int relationshipCount[maximum_relation]; 
int total_profiles = 0; 
int totalRelationships = 0; 
string uniqueRelations[maximum_relation];
int uniqueRelationCount = 0; 

string normalize(const string& str)
{
	string result = str;
	transform(result.begin(), result.end(), result.begin(), ::tolower);
	result.erase(result.find_last_not_of(" \t\n") + 1);
	result.erase(0, result.find_first_not_of(" \t\n"));
	size_t pos;
	while ((pos = result.find("  ")) != string::npos)
	{
		result.replace(pos, 2, " ");
	}
	return result;
}
vector<Profile> profiles_v;
vector<vector<int>> relationships_v;
vector<string> uniqueRelations_v;
vector<Profile> load_profile_v(const string& filename)
{
	ifstream file(filename);
	vector<Profile> profiles;

	if (!file.is_open()) {
		cerr << "ERROR TO OPEN FILE:" << filename << endl;
		return profiles;
	}

	string line;
	while (getline(file, line))
	{
		stringstream ss(line);
		string field;
		Profile profile_v;

		try
		{
			getline(ss, field, '\t');
			profile_v.id = stoi(field);

			getline(ss, field, '\t');
			profile_v.publicstatus = stoi(field);

			getline(ss, field, '\t');
			profile_v.age = stoi(field);

			getline(ss, field, '\t');
			profile_v.loggedin = stoi(field);

			getline(ss, field, '\t');
			profile_v.region = normalize(field);

			getline(ss, field, '\t');
			profile_v.last_login = field;

			getline(ss, field, '\t');
			profile_v.joined_date = field;

			getline(ss, field, '\t');
			profile_v.complication = stoi(field);

			profiles.push_back(profile_v);
		}
		catch (exception& e)
		{
			continue;
		}
	}

	file.close();
	cout << "PROFILES LOADED!" << endl;
	return profiles;
}
void load_profile(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "ERROR TO OPEN FILE:" << filename << endl;
		return;
	}

	string line;
	while (getline(file, line) && total_profiles < maximum_profile)
	{
		stringstream ss(line);
		string field;
		Profile profile;

		try
		{
			getline(ss, field, '\t');
			profile.id = stoi(field);

			getline(ss, field, '\t');
			profile.publicstatus = stoi(field);

			getline(ss, field, '\t');
			profile.age = stoi(field);

			getline(ss, field, '\t');
			profile.loggedin = stoi(field);

			getline(ss, field, '\t');
			profile.region = normalize(field);

			getline(ss, field, '\t');
			profile.last_login = field;

			getline(ss, field, '\t');
			profile.joined_date = field;

			getline(ss, field, '\t');
			profile.complication = stoi(field);

			profiles[total_profiles++] = profile; // Store profile in array
		}
		catch (exception& e)
		{
			continue;
		}
	}

	file.close();
	cout << "PROFILES LOADED!" << endl;
}

void load_relation_v(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		
		return;
	}

	string line;
	while (getline(file, line))
	{
		stringstream ss(line);
		int user1, user2;
		if (ss >> user1 >> user2)
		{
			string relation1 = to_string(min(user1, user2)) + "," + to_string(max(user1, user2));

		
			if (find(uniqueRelations_v.begin(), uniqueRelations_v.end(), relation1) == uniqueRelations_v.end())
			{
				uniqueRelations_v.push_back(relation1);  
			}

			
			if (user1 >= relationships_v.size())
				relationships_v.resize(user1 + 1);
			if (user2 >= relationships_v.size())
				relationships_v.resize(user2 + 1);

			
			relationships_v[user1].push_back(user2);
		}
	}
	file.close();
}
void load_relation(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open()) 
	{
		cout << "FILE CANNOT OPEN!" << endl;
		return;
	}

	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		int user1, user2;
		if (ss >> user1 >> user2) {
			relationships[user1][relationshipCount[user1]++] = user2;
		}
	}

	file.close();
	cout << "RELATION LOADED AS DIRECTED!" << endl;
}

struct Edge
{
	int u, v, weight;
	bool operator<(const Edge& other) const
	{
		return weight < other.weight;
	}
};

vector<Edge> generate_subgraph(const vector<vector<int>>& relationships, const vector<Profile>& profiles)
{
	vector<Edge> edges;

	for (size_t u = 0; u < relationships.size(); ++u)
	{
		for (size_t i = 0; i < relationships[u].size(); ++i)
		{
			int v = relationships[u][i];
			if (v < 0 || v >= profiles.size())
				continue;
			if (u < static_cast<size_t>(v))
			{
				int weight = abs(profiles[u].age - profiles[v].age);
				edges.push_back(Edge{ static_cast<int>(u), static_cast<int>(v), weight });
			}
		}
	}
	return edges;
}

class find_union
{
public:
	find_union(int n)
	{
		parent.resize(n);
		rank.resize(n, 0);
		for (int i = 0; i < n; ++i)
		{
			parent[i] = i;
		}
	}

	int find(int u)
	{
		if (parent[u] != u)
		{
			parent[u] = find(parent[u]);
		}
		return parent[u];
	}

	bool unionSets(int u, int v)
	{
		int rootU = find(u);
		int rootV = find(v);

		if (rootU != rootV)
		{
			if (rank[rootU] > rank[rootV])
			{
				parent[rootV] = rootU;
			}
			else if (rank[rootU] < rank[rootV])
			{
				parent[rootU] = rootV;
			}
			else {
				parent[rootV] = rootU;
				rank[rootU]++;
			}
			return true;
		}
		return false;
	}

private:
	vector<int> parent, rank;
};


vector<Edge> kruskal_mst(int n, const vector<Edge>& edges)
{
	if (edges.empty())
	{
		cout << "ERROR:NO EDGES TO PROCESS IN THE GRAPH" << endl;
		return{};
	}

	find_union uf(n);
	vector<Edge> mst;

	vector<Edge> sortedEdges = edges;
	sort(sortedEdges.begin(), sortedEdges.end());

	for (size_t i = 0; i < sortedEdges.size(); ++i)
	{
		const auto& edge = sortedEdges[i];
		if (uf.unionSets(edge.u, edge.v))
		{
			mst.push_back(edge);
		}
	}

	cout << "MST EDGES:" << endl;
	for (size_t i = 0; i < mst.size(); ++i)
	{
		const auto& edge = mst[i];
		cout << "Edge:(" << edge.u << ", " << edge.v << "), Weight:" << edge.weight << endl;
	}
	return mst;
}

bool is_reflexive()
{
	bool result = 1;
	int count = 0;
	for (int user = 1; user < relationships_v.size(); ++user)
	{
		bool self_relation = false;

		for (int i = 1; i < relationships_v[user].size(); ++i)
		{
			if (relationships_v[user][i] == user)
			{
				cout << "RELATION IS REFLEXIVE FOR USER:" << user << endl;
				self_relation = true;
				break;
			}

		}

		if (!self_relation)
		{
			cout << "NON RELATION FOUND FOR USER:" << user << endl;
			if (count < 1)
			{
				result = 0;
				count++;
			}
		}
	}
	if (result == 0)
	{
		cout << "=================================" << endl;
		cout << " FINAL RELATION IS NOT  REFLEXIVE" << endl;
		cout << "=================================" << endl;
		return false;
	}
	cout << "=================================" << endl;
	cout << "RELATION IS REFLEXIVE" << endl;
	cout << "=================================" << endl;
	return true;
}

bool check_transitive()
{
	bool transitive_found = false;
	cout << "---------------------" << endl;
	for (size_t user = 0; user < relationships_v.size(); ++user)
	{
		for (size_t i = 0; i < relationships_v[user].size(); ++i)
		{
			int friend_id = relationships_v[user][i];

			if (friend_id < relationships_v.size())
			{
				for (size_t j = 0; j < relationships_v[friend_id].size(); ++j)
				{
					int friendsFriendId = relationships_v[friend_id][j];


					if (friendsFriendId != user &&
						find(relationships_v[user].begin(), relationships_v[user].end(), friendsFriendId) != relationships_v[user].end()) {
						transitive_found = true;

						cout << "TRANSITIVE RELATION:" << user << " -> " << friend_id << " -> " << friendsFriendId << endl;
					}
				}
			}
		}

	}
	cout << "---------------------" << endl;
	if (transitive_found)
	{
		cout << "RELATION IS TRANSITIVE" << endl;
	}
	else {
		cout << "RELATION IS NON TRANSITIVE" << endl;
	}

	return transitive_found;
}



bool is_symmetric()
{
	for (size_t user = 0; user < relationships_v.size(); user++)
	{
		for (size_t i = 0; i < relationships_v[user].size(); i++)
		{
			int friend_id = relationships_v[user][i];
			bool reverseFound = 0;


			for (size_t j = 0; j < relationships_v[friend_id].size(); j++)
			{
				if (relationships_v[friend_id][j] == user)
				{
					reverseFound = 1;
					break;
				}
			}


			if (!reverseFound)
			{
				cout << "RELATION IS NOT SYMMETRIC:" << user << " -> " << friend_id
					<< " BUT NOT " << friend_id << " -> " << user << endl;
			}
			else {
				cout << "SYMMETRIC RELATION: " << user << " <-> " << friend_id << endl;
			}
		}
	}
	return true;
}

int count_above_threshold(int threshold)
{
	int count = 0;
	for (int i = 0; i < total_profiles; i++)
	{
		if (profiles[i].complication > threshold)
		{
			count++;
		}
	}
	return count;
}

bool prove_induction(int threshold, int X)
{
	if (total_profiles == 0 || threshold < 0)
	{
		cout << "Error: Invalid profiles or threshold value." << endl;
		return false;
	}
	if (total_profiles == 1)
	{
		int count = (profiles[0].complication > threshold) ? 1 : 0;
		cout << "BASE CASE: THE NUMBER OF USERS ABOVE THE THRESHOLD IS " << count << endl;
		if (count > X)
		{
			cout << "BASE CASE HOLDS! THE NUMBER OF USERS WITH COMPLICATION ABOVE " << threshold
				<< " IS GREATER THAN " << X << endl;
			return true;
		}
		else {
			cout << "BASE CASE FAILS! THE NUMBER OF USERS WITH COMPLICATION ABOVE " << threshold
				<< " IS NOT GREATER THAN " << X << endl;
			return false;
		}
	}

	// Inductive Hypothesis: Assume the statement holds for n profiles
	// Inductive Step: Check for n + 1 profiles
	int count = count_above_threshold(threshold);
	cout << "INDUCTIVE STEP: THE NUMBER OF USERS WITH COMPLICATION ABOVE " << threshold
		<< " FOR " << total_profiles << " PROFILES IS " << count << "." << endl;

	if (count > X)
	{
		cout << "INDUCTION HOLDS! THE NUMBER OF USERS WITH COMPLICATION ABOVE " << threshold
			<< " IS GREATER THAN " << X << endl;
		return true;
	}
	else
	{
		cout << "INDUCTION FAILS! THE NUMBER OF USERS WITH COMPLICATION ABOVE " << threshold
			<< " IS NOT GREATER THAN " << X << endl;
		return false;
	}
}

void preorder(int user, bool visited[])
{
	cout << user << " ";
	visited[user] = true;
	for (int i = 0; i < relationshipCount[user]; i++)
	{
		int friendId = relationships[user][i];
		if (!visited[friendId])
		{
			preorder(friendId, visited);
		}
	}
}

void postorder(int user, bool visited[])
{
	visited[user] = true;
	for (int i = 0; i < relationshipCount[user]; ++i)
	{
		int friendId = relationships[user][i];
		if (!visited[friendId])
		{
			postorder(friendId, visited);
		}
	}
	cout << user << " ";
}
void bfs(int start)
{
	bool visited[maximum_relation] = { false }; 
	int queue[maximum_relation];
	int front = 0, rear = 0;

	queue[rear++] = start;
	visited[start] = true;

	while (front < rear)
	{
		int user = queue[front++];
		cout << user << " ";

		// Add unvisited friends to the queue using index-based loop
		for (int i = 0; i < relationshipCount[user]; i++)
		{
			int friend_id = relationships[user][i];
			if (!visited[friend_id])
			{
				visited[friend_id] = true;
				queue[rear++] = friend_id;
			}
		}
	}
}

bool injective(const pair<int, int> user_completion[], int size)
{
	int seen[maximum_profile] = { 0 };
	for (int i = 0; i < size; i++)
	{
		int value = user_completion[i].second;
		if (seen[value])
		{
			return false;
		}
		seen[value] = 1;
	}
	return true;
}

bool is_surjective(const pair<int, int> user_completion[], int size, const int possible_val[], int possible_siz) {
	int range[maximum_profile] = { 0 };
	int range_size = 0;

	for (int i = 0; i < size; i++)
	{
		int value = user_completion[i].second;
		bool found = false;

		for (int j = 0; j < range_size; j++) {
			if (range[j] == value) {
				found = true;
				break;
			}
		}

		if (!found) {
			range[range_size++] = value;
		}
	}

	if (range_size != possible_siz) 
		return false;

	for (int i = 0; i < possible_siz; i++)
	{
		bool found = false;
		for (int j = 0; j < range_size; j++) {
			if (possible_val[i] == range[j]) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}

	return true;
}

void find_inverse(const pair<int, int> user_completion[], int size, pair<int, int> inverse_map[])
{
	for (int i = 0; i < size; i++) {
		int user = user_completion[i].first;
		int completion = user_completion[i].second;
		inverse_map[i] = { completion, user };
	}
}

void bubbleSort(int arr[], int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if (arr[j] > arr[j + 1]) {
				swap(arr[j], arr[j + 1]);
			}
		}
	}
}

void domain_range(const pair<int, int> user_completion[], int size) {
	int domain[maximum_profile], range[maximum_profile];
	int rangeSize = 0;
	for (int i = 0; i < size; i++)
	{
		domain[i] = user_completion[i].first;
		bool isUnique = true;
		for (int j = 0; j < rangeSize; j++) 
		{
			if (range[j] == user_completion[i].second)
			{
				isUnique = false;
				break;
			}
		}
		if (isUnique) {
			range[rangeSize++] = user_completion[i].second;
		}
	}
	bubbleSort(domain, size);
	bubbleSort(range, rangeSize);
	cout << "USER:DOMAIN-----COMPLICATION PERCENT:RANGE" << endl;
	cout << "DOMAIN:";
	for (int i = 0; i < size; i++)
	{
		cout << domain[i] << " ";
	}
	cout << endl;

	cout << "RANGE:";
	for (int i = 0; i < rangeSize; i++)
	{
		cout << range[i] << " ";
	}
	cout << endl;
}

void find_user_with_property(const string& region, int age_limit, int completion_limit)
{
	system("cls");
	cout << "-------------------" << endl;
	cout << "USER REGION:" << region << endl;
	cout << "Age > " << age_limit << endl;
	cout << "Completion > " << completion_limit << "%" << endl;
	bool found = false;

	for (int i = 0; i < total_profiles; ++i)
	{
		Profile& user = profiles[i];
		if (user.region == region && user.age > age_limit && user.complication > completion_limit)
		{
			user.display();
			found = true;
		}
	}

	if (!found)
	{
		cout << "NO USER FOUND!" << endl;
	}
	cout << "-------------------" << endl;
}

void check_user_completion_region(const string& region, int ageLimit, int completion_limit)
{
	cout << endl;
	cout << "----UNIVERSAL QUANTIFIER-----" << endl;
	bool all_meet_criteria = true;

	for (int i = 0; i < total_profiles; i++)
	{
		Profile& user = profiles[i];
		if (user.region == region && user.age > ageLimit && user.complication < completion_limit)
		{
			all_meet_criteria = false;
			cout << "USER " << user.id << " NOT MEET REQUIREMENT!" << endl;
		}
	}

	if (all_meet_criteria)
	{
		cout << "All USER OF " << region << " MEET REQUIREMENT " << endl;
	}
	cout << "--------------------------------------------" << endl;
	cout << endl;
}

int factorial(int n)
{
	long int result = 1;
	for (int i = 1; i <= n; ++i)
	{
		result *= i;
	}
	return result;
}

void permutations_completion_per()
{
	int n = total_profiles;
	cout << "PERMUTATION ON COMPLICATION:" << factorial(n) << endl;
}

int count_subgroup(int age_thresh, const string& region)
{
	int count = 0;
	for (int i = 0; i < total_profiles; ++i)
	{
		if (profiles[i].age > age_thresh && profiles[i].region == region)
		{
			count++;
		}
	}
	return count;
}

bool contains(const int vec[], int size, int val)
{
	for (int i = 0; i < size; i++)
	{
		if (vec[i] == val)
		{
			return true;
		}
	}
	return false;
}

string printset(const int set[], int size)
{
	stringstream ss;
	for (int i = 0; i < size; i++)
	{
		ss << set[i] << " ";
	}
	return ss.str();
}

void vendiagram(const string& reg, int age_thresh)
{
	int pub[maximum_profile], age[maximum_profile], reg_users[maximum_profile];
	int pub_age[maximum_profile], pub_reg[maximum_profile], age_reg[maximum_profile], all[maximum_profile];
	int pubCount = 0, ageCount = 0, regCount = 0, pubAgeCount = 0, pubRegCount = 0, ageRegCount = 0, allCount = 0;

	for (int i = 0; i < total_profiles; i++)
	{
		if (profiles[i].publicstatus == 1)
			pub[pubCount++] = profiles[i].id;
		if (profiles[i].age > age_thresh)
			age[ageCount++] = profiles[i].id;
		if (normalize(profiles[i].region) == normalize(reg))
			reg_users[regCount++] = profiles[i].id;
	}

	for (int i = 0; i < pubCount; i++)
	{
		if (contains(age, ageCount, pub[i]))
			pub_age[pubAgeCount++] = pub[i];
		if (contains(reg_users, regCount, pub[i]))
			pub_reg[pubRegCount++] = pub[i];
	}

	for (int i = 0; i < ageCount; i++)
	{
		if (contains(reg_users, regCount, age[i]))
			age_reg[ageRegCount++] = age[i];
	}

	for (int i = 0; i < pubAgeCount; i++)
	{
		if (contains(reg_users, regCount, pub_age[i]))
			all[allCount++] = pub_age[i];
	}

	cout << endl;
	cout << "*==================================================================*" << endl;
	cout << "Public Users Total:" << pubCount << " :{" << printset(pub, pubCount) << "}" << endl;
	cout << "Users above Total" << age_thresh << ":" << ageCount << " :{" << printset(age, ageCount) << "}" << endl;
	cout << "Users in Region Total " << regCount << " {" << printset(reg_users, regCount) << "}" << endl;
	cout << "Public n Age > " << age_thresh << ":Total" << pubAgeCount << " :{" << printset(pub_age, pubAgeCount) << "}" << endl;
	cout << "Public n Region:Total" << pubRegCount << " {" << printset(pub_reg, pubRegCount) << "}" << endl;
	cout << "Age > " << age_thresh << " n Region:" << ageRegCount << " :{" << printset(age_reg, ageRegCount) << "}" << endl;
	cout << "Public n Age > " << age_thresh << " n Region: Total" << allCount << " :{" << printset(all, allCount) << "}" << endl;
	cout << "*==================================================================*" << endl;
	cout << "|-------------------------VENDIAGRAM-DISCRIPTION-------------------|" << endl;
	cout << "*==================================================================*" << endl;
	cout << "PUBLIC: USERS WITH PUBLIC STATUS"<<endl;
	cout << "AGE > " << age_thresh << ": USERS OLDER THAN " << age_thresh << endl;
	cout << "REGION: USERS IN " << reg << "\n";
	cout << "PUBLIC n AGE > " << age_thresh << ": PUBLIC AND ABOVE AGE THRESHOLD" << endl;
	cout << "PUBLIC n REGION: PUBLIC AND IN THE REGION" << endl;
	cout << "AGE > " << age_thresh << " n REGION: AGE ABOVE THRESHOLD AND IN THE REGION" << endl;
	cout << "PUBLIC n AGE > " << age_thresh << " n REGION: ALL THREE CONDITIONS" << endl;
	cout << "*==================================================================*" << endl;
}
bool check_implies(const Profile& user)
{
	return (user.age > 30) ? (user.complication > 50) : true;
}

bool check_biconditionals(const Profile& user)
{
	return user.publicstatus == user.loggedin;
}
void check_propositions()
{
	int x;
	cout << "1.ALL USER PROPOSITION" << endl;
	cout << "2.FIND SPECIFIC USER PROPOSITION BY ID" << endl;
	cin >> x;
	if (x == 1)
	{
		for (int i = 0; i < total_profiles; i++)
		{
			const Profile& user = profiles[i];
			cout << "-------------------" << endl;
			cout << "ID:" << user.id << endl;
			cout << "IMPLICATION:";
			cout << (check_implies(user) ? "TRUE" : "FALSE") << endl;

			cout << "BICONDITIONAL:";
			cout << (check_biconditionals(user) ? "TRUE" : "FALSE") << endl;
			cout << "-------------------" << endl;
			cout << endl;
		}
	}
	else
	{
		cout << "INPUT ID:";
		cin >> x;
		for (int i = 0; i < total_profiles; i++)
		{
			const Profile& user = profiles[i];
			if (user.id == x)
			{
				cout << "*===================*" << endl;
				cout << "|ID:" << user.id << endl;
				cout << "|IMPLICATION:";
				cout << (check_implies(user) ? "TRUE" : "FALSE") << endl;

				cout << "|BICONDITIONAL:";
				cout << (check_biconditionals(user) ? "TRUE" : "FALSE") << endl;
				cout << "*===================*" << endl;
				cout << endl;
			}
		}
	}
}

void set_operations(const string& input)
{
	string region = normalize(input);
	int set_a[maximum_profile], set_b[maximum_profile];
	int unionSet[maximum_profile], intersection[maximum_profile], complement[maximum_profile];
	int size_a = 0, size_b = 0, sizeUnion = 0, size_intersection = 0, size_complement = 0;

	for (int i = 0; i < total_profiles; ++i)
	{
		if (profiles[i].publicstatus)
		{
			set_a[size_a++] = profiles[i].id;
		}
		if (normalize(profiles[i].region) == input)
		{
			set_b[size_b++] = profiles[i].id; 
		}
	}
	for (int i = 0; i < size_a; i++)
	{
		unionSet[sizeUnion++] = set_a[i];
	}
	for (int i = 0; i < size_b; i++)
	{
		if (find(unionSet, unionSet + sizeUnion, set_b[i]) == unionSet + sizeUnion)
		{
			unionSet[sizeUnion++] = set_b[i];
		}
	}
	for (int i = 0; i < size_a; i++)
	{
		if (find(set_b, set_b + size_b, set_a[i]) != set_b + size_b)
		{
			intersection[size_intersection++] = set_a[i];
		}
	}

	for (int i = 0; i < total_profiles; ++i)
	{
		if (find(set_a, set_a + size_a, profiles[i].id) == set_a + size_a)
		{
			complement[size_complement++] = profiles[i].id;
		}
	}

	cout << endl;
	cout << "-----------------------" << endl;
	cout << "Set A: {";
	for (int i = 0; i < size_a; ++i)
		cout << set_a[i] << (i < size_a - 1 ? ", " : "");
	cout << "}" << endl;

	
	cout << "Set B: {";
	for (int i = 0; i < size_b; ++i)
		cout << set_b[i] << (i < size_b - 1 ? ", " : "");
	cout << "}" << endl;

	
	cout << "A U B: {";
	for (int i = 0; i < sizeUnion; ++i)
		cout << unionSet[i] << (i < sizeUnion - 1 ? ", " : "");
	cout << "}" << endl;

	
	cout << "A n B: {";
	for (int i = 0; i < size_intersection; ++i)
		cout << intersection[i] << (i < size_intersection - 1 ? ", " : "");
	cout << "}" << endl;


	cout << "A': {";
	for (int i = 0; i < size_complement; ++i)
		cout << complement[i] << (i < size_complement - 1 ? ", " : "");
	cout << "}" << endl;
	cout << "-----------------------" << endl;
	cout << endl;
}


int count_profiles()
{
	return total_profiles;
}
void count_by_region()
{
	string region_count[maximum_profile];
	int region_count_values[maximum_profile] = { 0 };
	int region_count_size = 0;

	for (int i = 0; i < total_profiles; ++i)
	{
		bool region_found = false;
		for (int j = 0; j < region_count_size; ++j)
		{
			if (region_count[j] == profiles[i].region)
			{
				region_count_values[j]++;
				region_found = true;
				break;
			}
		}

		
		if (!region_found)
		{
			region_count[region_count_size] = profiles[i].region;
			region_count_values[region_count_size] = 1;
			region_count_size++;
		}
	}

	// Display the count of users in each region
	cout << "*=================================*" << endl;
	cout << "|  TOTAL  USER IN EACH REGION     |" << endl;
	cout << "*=================================*" << endl;
	for (int i = 0; i < region_count_size; ++i)
	{
		cout << "REGION:" << region_count[i] << endl;
		cout << "TOTAL USERS:" << region_count_values[i] << endl;
		cout << "-------------------------------" << endl;
	}
}

int count_profile_above(int n)
{
	int count = 0;
	for (int i = 0; i < total_profiles; ++i)
	{
		if (profiles[i].complication > n)
		{
			count++;
		}
	}
	return count;
}

void displays()
{
	int x;
	cout << "=============================" << endl;
	cout << "|          DISPLAY          |" << endl;
	cout << "=============================" << endl;
	cout << "|1.ALL USER                 |" << endl;
	cout << "|2.USER BY ID               |" << endl;
	cout << "|3.USER BY REGION           |" << endl;
	cout << "=============================" << endl;
	cin >> x;
	if (x == 1)
	{
		for (int i = 0; i < total_profiles; i++)
		{
			const Profile& user = profiles[i];
			user.display();
		}
	}
	else if (x == 2)
	{
		cout << "INPUT ID:";
		cin >> x;
		for (int i = 0; i < total_profiles; i++)
		{
			const Profile& user = profiles[i];
			if (user.id == x)
			{
				user.display();
			}
		}
	}
	else
	{
		string u;
		cout << "INPUT REGION:";
		cin.ignore();
		getline(cin, u);
		for (int i = 0; i < total_profiles; i++)
		{
			const Profile& user = profiles[i];
			if (user.region == u)
			{
				user.display();
			}
		}
	}
}
int main()
{
	string relationshipsFile = "relations.txt";
	string profilesFile = "profile.txt";

	load_relation_v(relationshipsFile);
	profiles_v = load_profile_v(profilesFile);
	load_profile(profilesFile);
	load_relation(relationshipsFile);
	system("color 0F");
	char choice;
	do
	{
		cout << "*=======================================*" << endl;
		cout << "|-----------DISCRETE-STRUCTURE----------|" << endl;
		cout << "|=======================================|" << endl;
		cout << "|A.EXIT                                 |" << endl;
		cout << "|B.DISPLAY PROFILES DATA                |" << endl;
		cout << "|C.EVALUATE PROPOSITIONS                |" << endl;
		cout << "|D.EXISTENTIAL QUANTIFIER               |" << endl;
		cout << "|E.UNIVERSAL QUANTIFIER                 |" << endl;
		cout << "|F.SET OPERATIONS                       |" << endl;
		cout << "|G.VENDIAGRAM                           |" << endl;
		cout << "|H.RELATIONS                            |" << endl;
		cout << "|I.FUNCTIONS                            |" << endl;
		cout << "|J.PERMUTATION COMBINATION              |" << endl;
		cout << "|K.COUNTING                             |" << endl;
		cout << "|L.TREE                                 |" << endl;
		cout << "|M.GRAPH                                |" << endl;
		cout << "|N.INDUCTION                            |" << endl;
		cout << "*---------------------------------------*" << endl;
		cin >> choice;
		choice = tolower(choice);
		if (choice == 'b')
		{
			system("cls");
			displays();
		}
		else if (choice == 'c')
		{
			system("cls");
			check_propositions();
		}
		else if (choice == 'd')
		{
			string region;
			int age_limit, completion_limit;
			cout << "INPUT REGION:";
			cin.ignore();
			getline(cin, region);
			region = normalize(region);
			cout << "INPUT AGE LIMIT:";
			cin >> age_limit;
			cin.ignore();
			cout << "INPUT COMPLICTION PERCENTAGE LIMIT:";
			cin >> completion_limit;
			find_user_with_property(region, age_limit, completion_limit);
		}
		else if (choice == 'e')
		{
			string region;
			int ageLimit, completionLimit;
			cout << "INPUT REGION:";
			cin.ignore();
			getline(cin, region);
			region = normalize(region);
			cout << "INPUT AGE LIMIT:";
			cin >> ageLimit;
			cout << "INPUT COMPLICTION PERCENTAGE LIMIT:";
			cin >> completionLimit;
			region = normalize(region);
			check_user_completion_region(region, ageLimit, completionLimit);
		}
		else if (choice == 'f')
		{
			system("cls");
			cout << "===============================" << endl;
			cout << "|        SET OPERATION        |" << endl;
			cout << "===============================" << endl;
			string region;
			cout << "INPUT REGION:";
			cin.ignore();
			getline(cin, region);
			region = normalize(region);
			set_operations(region);
		}
		else if (choice == 'g')
		{
			string region;
			cout << "INPUT REGION:";
			cin.ignore();
			getline(cin, region);
			region = normalize(region);
			int x;
			cout << "INPUT THRESHOLD:";
			cin >> x;
			vendiagram(region, x);
		}
		else if (choice == 'h')
		{
			cout << "*====================*" << endl;
			cout << "|      RELATION      |" << endl;
			cout << "*====================*" << endl;
			cout << "|1.IS SYMMETRIC      |" << endl;
			cout << "|2.IS TRANSITIVE     |" << endl;
			cout << "|3.IS REFLEXIVE      |" << endl;
			cout << "*--------------------*" << endl;
			char cx;
			cin >> cx;
			if (cx == '1')
			{
				is_symmetric();
			}
			else if (cx == '2')
			{
				check_transitive();
			}
			else if (cx == '3')
			{
				is_reflexive();
			}
		}
		else if (choice == 'i')
		{
			pair<int, int> userToCompletion[maximum_profile];
			for (int i = 0; i < total_profiles; i++) {
				userToCompletion[i] = { profiles[i].id, profiles[i].complication };
			}
			cout << "*=========================*" << endl;
			cout << "|       FUNCTIONS         |" << endl;
			cout << "*=========================*" << endl;
			cout << "|1.CHECK INJECTIVE        |" << endl;
			cout << "|2.CHECK SURJECTIVE       |" << endl;
			cout << "|3.DOMAIN AND RANGE       |" << endl;
			cout << "|4.INVERSE FUNCTION       |" << endl;
			cout << "--------------------------" << endl;
			char c;
			cin >> c;
			switch (c)
			{
			case '1': {
				if (injective(userToCompletion, total_profiles))
				{
					cout << "FUNCTION IS ONE TO ONE " << endl;
				}
				else
				{
					cout << "FUNCTION IS NOT ONE TO ONE " << endl;
				}
				break;
			}
			case '2':
			{
				int possibleValues[maximum_profile];
				for (int i = 0; i < total_profiles; i++) {
					possibleValues[i] = profiles[i].complication;
				}
				if (is_surjective(userToCompletion, total_profiles, possibleValues, total_profiles))
				{
					cout << "FUNCTION IS ON TO " << endl;
				}
				else {
					cout << "FUNCTION IS NOT ON TO " << endl;
				}
				break;
			}
			case '3':
			{
				domain_range(userToCompletion, total_profiles);
				break;
			}
			case '4':
			{
				int possibleValues[maximum_profile];
				for (int i = 0; i < total_profiles; i++) {
					possibleValues[i] = profiles[i].complication;
				}
				if (injective(userToCompletion, total_profiles) && is_surjective(userToCompletion, total_profiles, possibleValues, total_profiles))
				{
					cout << "FUNCTION IS BIJECTIVE." << endl;
					cout << "INVERSE MAPPING:" << endl;
					pair<int, int> inverse_map[maximum_profile];
					find_inverse(userToCompletion, total_profiles, inverse_map);
					for (int i = 0; i < total_profiles; i++)
					{
						cout << "COMPLICATION " << inverse_map[i].first << " -> USER " << inverse_map[i].second << endl;
					}
				}
				else
				{
					cout << "FUNCTION IS NOT BIJECTIVE SO INVERSE CANNOT BE DETERMINED!" << endl;
				}
				break;
			}
			}
		}
		else if (choice == 'j')
		{
			system("cls");
			int ag;
			string area;
			cout << "INPUT AGE:";
			cin >> ag;
			cin.ignore();
			cout << "INPUT REGION:";
			getline(cin, area);
			int count = count_subgroup(ag, area);
			cout << "TOTAL USER ABOVE AGE " << ag << " FROM " << area << ":" << count << endl;
			permutations_completion_per();
		}
		else if (choice == 'k')
		{
			cout << "*=========================*" << endl;
			cout << "|       COUNTING          |" << endl;
			cout << "*=========================*" << endl;
			cout << "|1.USERS ABOVE N COMPLETION" << endl;
			cout << "|2.USERS BY REGION" << endl;
			cout << "|3.PUBLIC PROFILES" << endl;
			cout << "--------------------------" << endl;

			char x;
			cin >> x;
			switch (x)
			{
			case '1':
			{
				int num;
				cout << "INPUT COMPLICATION:";
				cin >> num;
				cout << "*================*" << endl;
				int count = count_profile_above(num);
				cout << "TOTAL USER WITH COMPLETION ABOVE " << num << ":" << count << endl;

				cout << "*================*" << endl;
				break;
			}
			case '2':
			{
				count_by_region();
				break;
			}
			case '3':
			{
				cout << "*================*" << endl;
				int count = count_profiles();
				cout << "TOTAL PROFILE:" << count << endl;
				cout << "*================*" << endl;
				break;
			}
			}
		}
		else if (choice == 'l')
		{
			system("cls");
			cout << "*========================*" << endl;
			cout << "|          TREES         |" << endl;
			cout << "*========================*" << endl;
			cout << "|1.PREORDER TRAVERSAL    |" << endl;
			cout << "|2.POSTORDER TRAVERSAL   |" << endl;
			cout << "|3.BREADTH-FIRST SEARCH  |" << endl;
			cout << "*========================*" << endl;
			char y;
			cin >> y;
			switch (y)
			{
			case '1':
			{
				int startUser;
				cout << "INPUT USER ID:";
				cin >> startUser;

				if (startUser > 0)
				{
					bool visited[maximum_relation] = { false };
					cout << "PREORDER TRAVERSAL FROM USER " << startUser << ":";
					preorder(startUser, visited);
					cout << endl;
				}
				else
				{
					cout << "INVALID USER ID!" << endl;
				}
				break;
			}
			case '2':
			{
				int startUser;
				cout << "INPUT USER ID:";
				cin >> startUser;

				if (startUser > 0)
				{
					bool visited[maximum_relation] = { false };
					cout << "POSTORDER TRAVERSAL FROM USER " << startUser << ":";
					postorder(startUser, visited);
					cout << endl;
				}
				else
				{
					cout << "INVALID USER ID!" << endl;
				}
				break;
			}
			case '3':
			{
				int startUser;
				cout << "INPUT USER ID:";
				cin >> startUser;

				if (startUser >0)
				{
					cout << "BFS FROM USER " << startUser << ":";
					bfs(startUser);
					cout << endl;
				}
				else
				{
					cout << "INVALID USER ID!" << endl;
				}
				break;
			}
			}
		}
		else if (choice == 'm')
		{
			cout << "*========================*" << endl;
			cout << "|           MST         |" << endl;
			cout << "*========================*" << endl;


			vector<Edge> edges = generate_subgraph(relationships_v, profiles_v);
			vector<Edge> mst = kruskal_mst(profiles_v.size(), edges);

			int totalWeight = 0;
			for (size_t i = 0; i < mst.size(); ++i)
			{
				const auto& edge = mst[i];
				totalWeight += edge.weight;
			}
			cout << endl;
			cout << "*========================*" << endl;
			cout << " TOTAL MST WEIGHT:" << totalWeight << endl;
			cout << "*========================*" << endl;
			cout << endl;
		}
		else if (choice == 'n')
		{
			if (total_profiles == 0)
			{
				cout << "NO PROFILES LOADED!" << endl;
				continue;
			}
			int threshold;
			cout << "INPUT THRESHOLD:";
			cin >> threshold;
			int X;
			cout << "the value of X for comparison:";
			cin >> X;
			prove_induction(threshold, X);
		}
	} while (choice != 'a');
	return 0;
}
	
	

