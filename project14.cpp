#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <locale>

#include <algorithm> 

using namespace std;

class Task {
public:
    int id;
    string description;
    string created_time;
    bool is_done;

    Task(int task_id, const string& desc) : id(task_id), description(desc), is_done(false) {
        created_time = getCurrentTime();
    }

    Task() : id(0), description(""), is_done(false) {
        created_time = getCurrentTime();
    }

private:
    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;

#ifdef _MSC_VER
        tm tm_buf;
        localtime_s(&tm_buf, &time_t);
        ss << put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
#else
        ss << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S");
#endif
        return ss.str();
    }
};


class TodoManager {
private:
    vector<Task> tasks;
    int next_id;

public:
    TodoManager() : next_id(1) {
        loadTasks();
    }

    void run() {
        string input;
        cout << "=== 簡単ToDo ===" << endl;

        while (true) {
            cout << "\n1.追加 2.一覧 3.完了 4.CSV保存 5.終了: ";
            getline(cin, input);

            if (input == "1") addTask();
            else if (input == "2") listTasks();
            else if (input == "3") doneTask();
            else if (input == "4") saveToCSV();
            else if (input == "5") { saveTasks(); break; }
            else cout << "無効なコマンドです。" << endl;
        }
    }

    void addTask() {
        string desc;
        cout << "タスク内容: ";
        getline(cin, desc);
        if (!desc.empty()) {
            tasks.push_back(Task(next_id++, desc));
            cout << "追加完了!" << endl;
        }
    }

    void listTasks() {
        cout << "\n--- タスク一覧 ---" << endl;
        if (tasks.empty()) {
            cout << "タスクはありません。" << endl;
            return;
        }

       
        for (auto it = tasks.cbegin(); it != tasks.cend(); ++it) {
            cout << "[" << it->id << "] " << (it->is_done ? "✓" : "○")
                << " " << it->description << endl;
        }
    }

    void doneTask() {
        if (tasks.empty()) {
            cout << "タスクがありません。" << endl;
            return;
        }

        listTasks();
        cout << "完了するID: ";
        string input;
        getline(cin, input);

        try {
            int id_to_find = stoi(input);

            
            auto it = find_if(tasks.begin(), tasks.end(),
                [id_to_find](const Task& task) {
                    return task.id == id_to_find;
                });

            
            if (it != tasks.end()) {
                if (!it->is_done) {
                    it->is_done = true;
                    cout << "完了しました!" << endl;
                }
                else {
                    cout << "そのタスクは既に完了しています。" << endl;
                }
            }
            else {
                cout << "該当タスクが見つかりません。" << endl;
            }
        }
        catch (const std::invalid_argument& e) {
            cout << "無効な入力です。数字を入力してください。" << endl;
        }
        catch (const std::out_of_range& e) {
            cout << "入力された数字が大きすぎます。" << endl;
        }
    }

    void saveToCSV() {
        string filename = "todo_" + getCurrentTimeForFilename() + ".csv";
        ofstream file(filename);

        if (file.is_open()) {
            file << "ID,状態,作成日時,内容" << endl;

            
            for (auto it = tasks.cbegin(); it != tasks.cend(); ++it) {
                file << it->id << "," << (it->is_done ? "完了" : "未完")
                    << "," << it->created_time << ",\"" << it->description << "\"" << endl;
            }
            file.close();
            cout << "保存完了: " << filename << endl;
        }
    }

    void saveTasks() {
        ofstream file("todo.txt");
        if (file.is_open()) {

           
            for (auto it = tasks.cbegin(); it != tasks.cend(); ++it) {
                file << it->id << "|" << it->description << "|"
                    << it->created_time << "|" << it->is_done << endl;
            }
            file.close();
        }
    }

    void loadTasks() {
        ifstream file("todo.txt");
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string idStr, desc, time, doneStr;

            if (getline(ss, idStr, '|') && getline(ss, desc, '|') &&
                getline(ss, time, '|') && getline(ss, doneStr)) {

                Task task;
                try {
                    task.id = stoi(idStr);
                    task.description = desc;
                    task.created_time = time;
                    task.is_done = (doneStr == "1");
                    tasks.push_back(task);

                    if (task.id >= next_id) next_id = task.id + 1;
                }
                catch (...) {
                    
                }
            }
        }
        file.close();
    }

private:
    string getCurrentTimeForFilename() {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;

#ifdef _MSC_VER
        tm tm_buf;
        localtime_s(&tm_buf, &time_t);
        ss << put_time(&tm_buf, "%Y%m%d_%H%M%S");
#else
        ss << put_time(localtime(&time_t), "%Y%m%d_%H%M%S");
#endif
        return ss.str();
    }
};

int main() {
    setlocale(LC_ALL, "Japanese");

    TodoManager manager;
    manager.run();
    return 0;
}