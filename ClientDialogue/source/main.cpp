#include <Client/Client.hpp>

int main() 
{
    Client client;
    while (true) 
    {
        std::string command;
        std::cout << "Enter command (e.g., 'add 5 10'): ";
        std::getline(std::cin, command);

        if (!command.empty()) 
            client.send_command(command);
        else
            std::cout << "Empty command. Please try again\n";
    }
    return 0;
}