#include <iostream>

#include <json/json.h>

#include "JsonCast.h"
#include "Person.h"

class Unregistered
{ };

#include <Meta.h>

void printSeparator()
{
    std::cout << "========================\n";
}


int main()
{
    Person person;
    person.age = 25;
    person.salary = 3.50f;
    person.name = "Alex"; // I'm a person!
    person.type = PersonType::ATTENDEE; // I'm paying for all these movies!

    person.favouriteMovies["Nostalgia Critic"] = { MovieInfo{ "The Room", 8.5f } };
    person.favouriteMovies["John Tron"] = { MovieInfo{ "Goosebumps", 10.0f },
                                            MovieInfo{ "Talking Cat", 9.0f } };

    // printing members of different classes
    std::cout << "Members of class Person:\n";
    meta::doForAllMembers<Person>(
        [](const auto& member)
        {
            std::cout << "* " << member.getName() << '\n';
        }
    );

    std::cout << "Members of class MovieInfo:\n";
    meta::doForAllMembers<MovieInfo>(
        [](const auto& member)
        {
            std::cout << "* " << member.getName() << '\n';
        }
    );

    printSeparator();

    // checking if classes are registered
    if (meta::isRegistered<Person>()) {
        std::cout << "Person class is registered\n";
    }

    // meta::isRegistered is constexpr, so can be used in enable_if and static_assert!
    static_assert(meta::isRegistered<Person>(), "Person class is not registered!");

    if (!meta::isRegistered<Unregistered>()) {
        std::cout << "Unregistered class is unregistered\n";
    }

    // checking if class has a member
    if (meta::hasMember<Person>("age")) {
        std::cout << "Person has member named 'age'\n";
    }

    // getting setting member values
    auto name = meta::getMemberValue<std::string>(person, "name");
    std::cout << "Got person's name: " << name << '\n';

    meta::setMemberValue<std::string>(person, "name", "Ron Burgundy");
    name = meta::getMemberValue<std::string>(person, "name");
    std::cout << "Changed person's name to " << name << '\n';


    // getting setting enum member values
    auto type = meta::getEnumMemberValueString<PersonType>(person, "type");
    std::cout << "Got person's type: " << type << '\n';

    // Set person's type to some real value
    meta::setEnumMemberValueString<PersonType>(person, "type", "Employee");
    type = meta::getEnumMemberValueString<PersonType>(person, "type");
    std::cout << "Changed person's type: " << type << '\n';



    printSeparator();

    // And here's how you can serialize/deserialize
    // (if you write a function for your type. I wrote it for Json::Value! ;D)
    std::cout << "Serializing person:" << '\n';
    Json::Value root = Json::serialize(person);
    std::cout << root << std::endl;

    Unregistered y;
    Json::Value root2 = Json::serialize(y);
    std::cout << "Trying to serialize unregistered class:\n";
    std::cout << root2 << '\n';

    printSeparator();

    std::cout << "Serializing Person 2 from JSON:\n";

    auto person2 = Json::deserialize<Person>(root);
    std::cout << "Person 2 name is " << person2.getName() << " too!" << '\n';

#ifdef _WIN32 // okay, this is not cool code, sorry :D
    system("pause");
#endif
}