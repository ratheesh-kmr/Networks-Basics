#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

class Team {
public:
    string name;
    int score;

    Team(string teamName) {
        name = teamName;
        score = 0;
    }

    void addScore(int marks) {
        score += marks;
    }
};

void multicastQuestion(const string& question, const vector<Team>& teams) {
    cout << "Multicasting question: " << question << endl;
    for (const auto& team : teams) {
        cout << "Team " << team.name << " received the question.\n";
    }
    cout << "Waiting for teams to answer...\n";
    this_thread::sleep_for(chrono::seconds(3));  // Simulating time to answer
}

bool teamAnswersCorrectly() {
    return rand() % 2;  // Randomly decide if the answer is correct
}

void displayScores(const vector<Team>& teams) {
    cout << "\n--- Final Scores ---" << endl;
    for (const auto& team : teams) {
        cout << team.name << ": " << team.score << " points" << endl;
    }
}

void startQuiz(int rounds, int timeLimit, int marksPerQuestion) {
    vector<Team> teams = {Team("A"), Team("B"), Team("C"), Team("D"), Team("E")};
    srand(time(0));  // Seed random number generator

    // Start quiz rounds
    for (int round = 1; round <= rounds; round++) {
        cout << "\n--- Round " << round << " ---\n";

        // Round-robin question distribution
        for (int i = 0; i < teams.size(); i++) {
            string question = "[Question " + to_string(round) + " for Team " + teams[i].name + "]";
            multicastQuestion(question, teams);  // Multicast question

            // Simulate team answering
            cout << "Team " << teams[i].name << " answering..." << endl;
            if (teamAnswersCorrectly()) {
                cout << "Team " << teams[i].name << " answered correctly!\n";
                teams[i].addScore(marksPerQuestion);
            } else {
                cout << "Team " << teams[i].name << " answered incorrectly.\n";
            }

            // Simulate waiting for the time limit
            cout << "Time is up for Team " << teams[i].name << "!\n\n";
        }
    }

    // Announce final scores
    displayScores(teams);
}

int main() {
    int rounds, timeLimit, marksPerQuestion;

    cout << "Welcome to the Multicast e-Quiz!\n";
    cout << "Enter the number of rounds: ";
    cin >> rounds;
    cout << "Enter time limit per question (in seconds): ";
    cin >> timeLimit;
    cout << "Enter marks per question: ";
    cin >> marksPerQuestion;

    // Start the quiz
    startQuiz(rounds, timeLimit, marksPerQuestion);

    return 0;
}
