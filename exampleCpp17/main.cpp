#include <string>
#include <optional>
#include <vector>
#include <iostream>

#include "Meta.h"
#include "JsonCast.h"

struct Person
{
   std::string        name;
   int                age;
   std::optional<int> weight;
};

std::ostream& operator<<(std::ostream& o, const Person& p)
{
   o << "name:" << p.name << std::endl;
   o << "age:" << p.age << std::endl;
   if(p.weight)
   {
      o << "weight" << *p.weight << std::endl;
   }
   return o;
}

struct Triangle
{
   float a, b, c;
};

std::ostream& operator<<(std::ostream& o, const Triangle& t)
{
   o << "a:" << t.a << " b:" << t.b << " c:" << t.c;
   return o;  
}

namespace meta
{

template <>
inline auto registerMembers<Person>()
{
   return members(
      member("name", &Person::name),
      member("age", &Person::age),
      member("weight", &Person::weight)
   );
}

template <>
inline auto registerMembers<Triangle>()
{
   return members(
      member("a", &Triangle::a),
      member("b", &Triangle::b),
      member("c", &Triangle::c)
   );
}

} // namespace meta

//- -- This is just needed for the lambda overload collection
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;
//-----------------------------------------------------------

int main()
{
   // OPTIONALS
   {
      // serialize empty optional
      Person person({"Dodo", 39});
      nlohmann::json jPerson = person;
      std::cout << jPerson.dump(1) << std::endl;
   }
   {
      // serialize optional with value
      Person person({"Dodo", 39});
      person.weight.emplace(98);
      nlohmann::json jPerson = person;
      std::cout << jPerson.dump(1) << std::endl;
   }
   {
      // deserialize missing optional
      auto j = nlohmann::json::parse("{\"name\":\"Dodo\",\"age\":20}");
      auto person = j.get<Person>();
      std::cout << person << std::endl;
   }
   {
      // deserialize optional with value
      auto j = nlohmann::json::parse("{\"name\":\"Dodo\",\"age\":20,\"weight\":98}");
      auto person = j.get<Person>();
      std::cout << person << std::endl;
   }

   // VARIANTS
   {
      using AllInOne = std::variant<Person, Triangle>;

      // Serialize
      std::vector<AllInOne> vectorOfVariants;
      vectorOfVariants.emplace_back(Person({"Dodo", 39}));
      vectorOfVariants.emplace_back(Triangle({10.0, 20.0, 30.0}));
      nlohmann::json jVectorOfVariants(vectorOfVariants);
      std::cout << jVectorOfVariants.dump(1) << std::endl;

      // Deserialize
      std::vector<AllInOne> vReadBack;
      jVectorOfVariants.get_to(vReadBack);
      for(auto& element : vReadBack)
      {
         std::visit(overload{
            [](const Person& obj) { std::cout << obj << std::endl; },
            [](const Triangle& obj) { std::cout << obj << std::endl; }
         }, element);
      }
   }

   return 0;
}