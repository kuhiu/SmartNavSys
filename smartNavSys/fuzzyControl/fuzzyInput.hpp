#ifndef __FUZZYINPUT_H__
#define __FUZZYINPUT_H__

#define UPPER_LIMIT 1.0

#include <fuzzyMembership.hpp>

#include <string>
#include <vector>
#include <cmath>

class fuzzyInput {
public:
    fuzzyInput() = default;
    ~fuzzyInput() = default;
    
    void initializer(const std::string &name, std::vector<fuzzyMembership> &memberships);
    void fuzzyfication(void);
    const std::string &getName(void) const;
    std::vector<fuzzyMembership> &getMemberships(void);
    void setInput(int input);
private:
    std::string __name;
    std::vector<fuzzyMembership> __memberships;
    int __input;
    void __compute_degree_of_membership(fuzzyMembership &mf, int input);
};


#endif // __FUZZYINPUT_H__