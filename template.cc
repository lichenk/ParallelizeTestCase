#include <bits/stdc++.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
extern char **environ;
using namespace std;
typedef long long ll;
typedef long double ld;
#define PB push_back
#define MP make_pair
#define MOD 1000000007LL
#define endl "\n"
#define fst first
#define snd second
const ll UNDEF = -1;
const ll INF=1LL<<61;
template<typename T> inline bool chkmax(T &aa, T bb) { return aa < bb ? aa = bb, true : false; }
template<typename T> inline bool chkmin(T &aa, T bb) { return aa > bb ? aa = bb, true : false; }
typedef pair<ll,ll> pll;
typedef vector<ll> vll;
typedef vector<int> vi;
typedef pair<int,int> pii;
string STDOUT_RESERVED = "STDOUT_RESERVED";
/*
This solves the following problem:
You are given the number of test cases T
You are given an integer n, followed by n integers a_i.
Please output the sum of the n integers as "Case #TEST_CASE_NUMBER: SUM\n".

Instructions:
Step 1: Write code to parse one test case in readOneTestCase().
Step 2: Write your code in solve().
Step 3: Compile and run with no arguments. Pass test data into stdin. Results are passed to stdout.
Step 4: If you want to debug test case X, run the executable with X as its only argument, where X is some positive integer.
To modify the input test case, look for testcaseinX.
Step 5: If you modify the original test file, be sure to run the executable with no arguments to regenerate the partitioned input files.

Notes: Your code does not have to clear global state for each test case. Each test case is run in a brand new process.

How does this work?
Let ./executable be the name of this executable.
When you run ./executable, it reads and parses all test cases from stdin, and writes each test case into an individual input file.
Then ./executable spawns various new processes with the command line "./executable X", where X is the test case number to run.
"./executable X" reads an individual test case, and writes the solution to a file dedicated to that test case.
After the child processses have finished execution, then the main parent process will reads all the solutions from the files written
by the child processes, and print them out in order.
*/

int solve(string testcasein, string testcaseout, int testnum)
{
	/*
	TO THE USER: You must modify this code!
	Solve a test case from the file named testcasein, and writes answer to the file named testcaseout.
	To make debugging easier, if testcaseout is STDOUT_RESERVED, you want to write the answer to stdout.
	*/
	std::ifstream ifs(testcasein.c_str(),std::ifstream::in);
	FILE *fout;
	// Make debugging individual test case easier: Pipe answers to stdout
	if (testcaseout==STDOUT_RESERVED) fout = stdout;
	else fout=fopen(testcaseout.c_str(),"wb");
	assert(fout!=NULL);
	ll n; ifs>>n;
	ll sum=0;
	for (int i=0;i<n;i++) {ll x; ifs>>x; sum+=x;}
	fprintf(fout, "Case #%d: %lld\n",testnum,sum);
}

void readOneTestCase(string testcaseout) {
	/*
	TO THE USER: You must modify this code!
	Read one test case, and output to a file named testcaseout
	*/
	ll n;
	cin>>n;
	vector<ll> tmp;
	for (int i=0;i<n;i++) {ll x; cin>>x; tmp.PB(x);}
	FILE *fp = fopen(testcaseout.c_str(),"wb");
	fprintf(fp, "%lld\n",n);
	for (auto &w:tmp) {
		fprintf(fp, "%lld ",w);
	}
	fprintf(fp, "\n");
	fclose(fp);
	/* End custom code! */
}

const int MAX_NUM_PROCESSES = 4;

string getinputfilename(int testnum) {
	ostringstream testcasein; testcasein<<"testcasein"<<testnum;
	return testcasein.str();
}
string getoutputfilename(int testnum) {
	ostringstream testcaseout; testcaseout<<"testcaseout"<<testnum;
	return testcaseout.str();
}

void wait_for_running_children_to_be_under_limit(set<pid_t> &running_child_pids, int limit) {
	while(running_child_pids.size() > limit) {
		for (auto &ChildPID:running_child_pids) {
			int status;
			pid_t result = waitpid(ChildPID, &status, WNOHANG);
			if (result == 0) {
			  // Child still alive
			} else if (result == -1) {
			  // Error 
			  cerr<<"waitpid error"<<endl; assert(0);
			} else {
			  // Child exited
			  if (status!=0) {
			  	cerr<<"Child returned nonzero status."<<endl; assert(0);
			  }
			  running_child_pids.erase(ChildPID);
			}
		}
		sleep(1); // TODO: Use condition variables.
	}	
}

int main(int argc, char* argv[])
{
	/*
	If no arguments are passed, then please pass full data set into stdin
	If one argument is passed, that argument should be the test case number. It will read from getinputfilename(testnum)
	and write to getoutputfilename(testnum). Usually, you first run this with no arguments to populate all the input files,
	then you pass in the test case number as the argument to debug this particular test case.
	*/
	ios_base::sync_with_stdio(false); cin.tie(0);
	set<pid_t> running_child_pids;
	if (argc == 1)
	{
		int number_of_test_cases; cin>>number_of_test_cases;
		for (int testnum=1;testnum<=number_of_test_cases;testnum++) {
			readOneTestCase(getinputfilename(testnum));

			// Execute child process with test case number as its only argument, plus one additional dummy argument
			ostringstream childCommandLine; childCommandLine << argv[0] << " " << testnum << " 0";
			pid_t pid=fork();
			running_child_pids.insert(pid);
			int exitCode,status;
			switch(pid) {
				case -1:
					cerr<<"Fork failed"<<endl; assert(0);
					return 0;
					break;
				case 0:
					/*
					Start a new process by running this executable with the test case number as an argument.
					*/
					exitCode = system(childCommandLine.str().c_str());
					if (exitCode!=0) {
						cerr<<"Runtime error"<<endl;
						assert(0);
					}
					assert(exitCode==0);
					return 0;
					break;
				default:
					// Parent process. Continue execution below.
					break;
			}
			wait_for_running_children_to_be_under_limit(running_child_pids, MAX_NUM_PROCESSES);
		}
		// Wait for processes to finish executing
		wait_for_running_children_to_be_under_limit(running_child_pids, 0);

		for (int testnum=1;testnum<=number_of_test_cases;testnum++) {
			std::ifstream ifs(getoutputfilename(testnum).c_str(),std::ifstream::in);
			string str;
			while(getline(ifs,str)) {
				if (ifs.bad()) {cerr<<"ifs bad"<<endl; assert(0);}
				cout<<str<<endl;
				if (ifs.eof()) break;
			}
		}
	}
	else {
		int testnum; istringstream(argv[1]) >> testnum;
		ostringstream testcasein; testcasein<<"testcasein"<<testnum;
		ostringstream testcaseout; testcaseout<<"testcaseout"<<testnum;
		// If only testnum is passed, then print results to stdout for debugging.
		if (argc==2) solve(getinputfilename(testnum), STDOUT_RESERVED, testnum);
		else solve(getinputfilename(testnum),getoutputfilename(testnum), testnum);
	}
}
