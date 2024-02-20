#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"
#include <random>

using namespace std;

using json = nlohmann::json;

class TextAdventureGame {
public:
    void loadMap(const string &filename);

    void startGame();

    vector<string> playerInventory;

private:
    json jsonData;
    string currentPlayerRoom;

    void playerMovement(const string &direction);

    void printDesc(const string &objectName);

    void printEnemyDesc(const string &enemyName);

    void printRoomDescription(const string &roomId);

    void printObjectDescription(const string &objectName);

    void handleTakeCommand(const string &objectName);

    bool checkObjective();

    void displayInventory();

    void killCommand(const string &enemy);
    void checkForDeath();

    void quitGame();
};

void TextAdventureGame::loadMap(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file '" << filename << "'" << endl;
        return;
    }

    try {
        file >> jsonData;
    } catch (const exception &e) {
        cerr << "Error while parsing JSON: " << e.what() << endl;
        file.close();
        return;
    }

    file.close();
}

void TextAdventureGame::startGame() {
    currentPlayerRoom = jsonData["player"]["initialroom"];
    printRoomDescription(currentPlayerRoom);

    while (true) {
        string command;
        cout << ">";
        getline(cin, command);

        if (command == "quit") {
            quitGame();
            break;
        } else if (command.substr(0, 4) == "take") {
            if (command == "take") {
                cout << "What do you want to take?" << endl;
            } else {
                string itemName = command.substr(5);
                handleTakeCommand(itemName);
                checkObjective();
                displayInventory();
            }
        } else if (command == "look" || command == "look around") {
            printRoomDescription(currentPlayerRoom);
        } else if (command.substr(0, 4) == "look") {
            string itemName = command.substr(5);
            printDesc(itemName);
        } else if (command.substr(0, 2) == "go") {
            if (command == "go") {
                cout << "Where are you going?" << endl;
            } else {
                string dirCommand = command.substr(3);
                checkForDeath();
                playerMovement(dirCommand);
                checkObjective();
            }
        } else if (command.substr(0, 4) == "kill") {
            string enemy = command.substr(5);
            killCommand(enemy);
            checkObjective();
        } else {
            cout << "Invalid command. Try again." << endl;
        }
    }
}

bool TextAdventureGame::checkObjective() {
    auto &objective = jsonData["objective"];

    if (objective["type"] == "room") {
        for (const auto &room : objective["what"]) {
            if (room == currentPlayerRoom) {
                cout << "Congratulations! You have completed the objective." << endl;
                quitGame();
                return true;
            }
        }
    } else if (objective["type"] == "collect") {
        auto &objects = jsonData["objects"];
        for (const auto &item : objective["what"]) {
            if (find(playerInventory.begin(), playerInventory.end(), item) == playerInventory.end()) {
                return false;
            }
        }

        cout << "Congratulations! You have collected all required items." << endl;
        quitGame();
        return true;
    } else if (objective["type"] == "kill") {
        auto &enemies = jsonData["enemies"];
        for (const auto &enemy : objective["what"]) {
            for (const auto &e : enemies) {
                if (e["id"] == enemy) {
                    return false;
                }
            }
        }

        cout << "Congratulations! You have killed all required enemies." << endl;
        quitGame();
        return true;
    }

    return false;
}

void TextAdventureGame::killCommand(const string &enemy) {
    for (const auto &enemyData : jsonData["enemies"]) {
        if (enemyData["id"] == enemy) {
            if (enemyData["initialroom"] == currentPlayerRoom) {
                cout << enemyData["intro_msg"] << endl;

                if (enemyData.find("successful_kill_msg") != enemyData.end()) {
                    cout << enemy << " " << enemyData["successful_kill_msg"] << endl;
                } else {
                    cout << "You have successfully defeated " << enemy << "." << endl;
                }

                string enemyToRemove = enemy;

                if (jsonData.find("enemies") != jsonData.end() && jsonData["enemies"].is_array()) {
                    auto &enemies = jsonData["enemies"];
                    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
                        if ((*it)["id"] == enemyToRemove) {
                            it = enemies.erase(it);
                            break;
                        }
                    }
                }
                return;
            } else {
                cout << enemy << " is not in this room or does not exist" << endl;
            }
        }
    }
}

void TextAdventureGame::checkForDeath() {
    for (const auto &enemy : jsonData["enemies"]) {
        if (enemy["initialroom"] == currentPlayerRoom) {
            int agg = enemy["aggressiveness"];
            if (agg == 0) {
                return;
            } else {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<int> distribution(0, 100);
                int random_number = distribution(gen);
                if (agg >= random_number) {
                    cout << "You have been killed by " << enemy["id"] << endl;
                    quitGame();
                }
            }
        }
    }
}

void TextAdventureGame::printDesc(const string &objectName) {
    for (const auto &object : jsonData["objects"]) {
        if (object["id"] == objectName) {
            printObjectDescription(objectName);
            return;
        }
    }

    for (const auto &object : jsonData["enemies"]) {
        if (object["id"] == objectName) {
            printEnemyDesc(objectName);
            return;
        }
    }

    cout << "Object or enemy not found: " << objectName << endl;
}

void TextAdventureGame::printObjectDescription(const string &objectName) {
    if (playerInventory.empty()) {
        for (const auto &object : jsonData["objects"]) {
            if (object["initialroom"] == currentPlayerRoom && object["id"] == objectName) {
                cout << object["desc"] << endl;
                return;
            }
        }
    } else {
        for (const auto &item : playerInventory) {
            if (item == objectName) {
                for (const auto &object : jsonData["objects"]) {
                    if (object["id"] == objectName) {
                        cout << object["desc"] << endl;
                        return;
                    }
                }
            }
        }
    }

    cout << "Object not found: " << objectName << endl;
}

void TextAdventureGame::printEnemyDesc(const string &enemyName) {
    for (const auto &enemy : jsonData["enemies"]) {
        if (enemy["initialroom"] == currentPlayerRoom && enemy["id"] == enemyName) {
            cout << enemy["desc"] << endl;
            return;
        }
    }

    cout << "Enemy not found: " << enemyName << endl;
}

void TextAdventureGame::printRoomDescription(const string &roomId) {
    for (const auto &room : jsonData["rooms"]) {
        if (room["id"] == roomId) {
            cout << room["desc"] << endl;

            for (const auto &object : jsonData["objects"]) {
                if (object["initialroom"] == roomId) {
                    bool foundInInventory = false;

                    for (const auto &item : playerInventory) {
                        if (item == object["id"]) {
                            foundInInventory = true;
                            break;
                        }
                    }

                    if (!foundInInventory) {
                        cout << "You see a " << object["id"] << " in the room." << endl;
                    }
                }
            }

            for (const auto &enemy : jsonData["enemies"]) {
                if (enemy["initialroom"] == roomId) {
                    cout << enemy["intro_msg"] << endl;
                }
            }

            return;
        }
    }

    cerr << "Error: Room not found." << endl;
}

void TextAdventureGame::handleTakeCommand(const string &objectName) {
    for (const auto &object : jsonData["objects"]) {
        if (object["initialroom"] == currentPlayerRoom && object["id"] == objectName) {
            if (std::find(playerInventory.begin(), playerInventory.end(), objectName) != playerInventory.end()) {
                cout << "You already have the " << objectName << "." << endl;
                return;
            }

            cout << "You take the " << objectName << "." << endl;
            playerInventory.push_back(objectName);
            return;
        }
    }

    cout << "Error: The specified object is not in the room." << endl;
}

void TextAdventureGame::displayInventory() {
    cout << "Inventory: ";
    if (playerInventory.empty()) {
        cout << "Empty";
    } else {
        for (const auto &item : playerInventory) {
            cout << item << ", ";
        }
    }
    cout << endl;
}

void TextAdventureGame::playerMovement(const string &direction) {
    if (!jsonData["rooms"].is_array()) {
        cerr << "Error: 'rooms' is not an array." << endl;
        return;
    }

    for (const auto &room : jsonData["rooms"]) {
        if (!room.is_object()) {
            cerr << "Error: Room entry is not an object." << endl;
            continue;
        }

        if (room["id"] == currentPlayerRoom) {
            if (room.find("exits") != room.end() && room["exits"].is_object()) {
                const auto &exits = room["exits"];

                for (const auto &exit : exits.items()) {
                }
                cout << endl;

                if (exits.find(direction) != exits.end()) {
                    currentPlayerRoom = exits[direction];
                    printRoomDescription(currentPlayerRoom);
                    return;
                } else {
                    cout << "Invalid exit or direction." << endl;
                    return;
                }
            } else {
                cout << "Error: 'exits' field not found or not an object." << endl;
                return;
            }
        }
    }

    cerr << "Error: Current room not found in the map data." << endl;
}

void TextAdventureGame::quitGame() {
    cout << "Game Over" << endl;
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <map_file>" << endl;
        return 1;
    }

    TextAdventureGame game;
    game.loadMap(argv[1]);
    game.startGame();
    return 0;
}