
#include "testu01_mt.h"
#include "bigcrush.h"
#include "crush.h"
#include "smallcrush.h"
#include "generators.h"

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <map>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>

//////////////////////////////////////////////////////////////////////

template<size_t lfib_a, size_t lfib_b>
class LFibGenerator : public UniformGenerator
{
    static constexpr double c = 5566755282872655.0 / 9007199254740992.0; ///< shift
    static constexpr double r = 9007199254740881.0 / 9007199254740992.0; ///< base (prime)

    double z;
    double w;
    double U[lfib_a + 1];
    int i;
    int j;

    inline double amb_mod_r(double a, double b)
    {
        double x = a - b;
        return (x >= 0.0) ? x : (x + r);
    }

public:
    LFibGenerator(uint32_t seed) : UniformGenerator("LFIB")
    {    
        z = (double) seed / UINT_MAX;
        w = z;
        double w2 = z, w3 = z;
        for (size_t k = 1; k <= lfib_a; k++) {
            z = amb_mod_r(z, c);
            w = amb_mod_r(w, z);
            w2 = amb_mod_r(w2, w);
            w3 = amb_mod_r(w3, w2);
            U[k] = w3;
        }
        i = lfib_a; j = lfib_b;
    }

    double GetU01()
    {
        double x = U[i] - U[j];
        if (x < 0.0) x += 1.0;
        U[i] = x;
	    if(--i == 0) i = lfib_a;
    	if(--j == 0) j = lfib_a;
        z = amb_mod_r(z, c);
        w = amb_mod_r(w, z);
        x -= w;
        return (x < 0.0) ? (x + 1.0) : x;
    }

    uint32_t GetBits()
    {
	    return (uint32_t) (GetU01() * unif01_NORM32);
    }
};



////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
    const std::map<std::string, GenFactoryFunc> gen_map = {
        {"LCG", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator());
        }},
        {"LCG59", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator59());
        }},
        {"LFIBMUL-17-5", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LFibMulGenerator<17,5>());
        }},
        {"KISS93", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new KISS93Generator());
        }},
        {"MT19937", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new MT19937Generator());
        }},
        {"SPLITMIX", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new SplitMixGenerator());
        }}
    };

    if (argc != 3) {
        printf("Usage: mt_test battery generator\n");
        printf("battery: battery name (SmallCrush, Crush, BigCrush)\n");
        printf("generator: PRNG name. The supported generators are:\n");
        std::vector<std::string> gnames;
        for (auto &kv : gen_map) {
            gnames.push_back(kv.first);
        }

        std::sort(gnames.begin(), gnames.end());
        for (auto &n : gnames) {
            printf("  %s\n", n.c_str());
        }
        return 0;
    }

    std::string battery(argv[1]), generator(argv[2]);

    if (gen_map.count(generator) == 0) {
        std::cerr << "Unknown generator " << generator << std::endl;
        return 1;
    }

    auto create_gen = gen_map.at(generator);    
    if (battery == "SmallCrush") {
        mt_bat_SmallCrush(create_gen);
    } else if (battery == "Crush") {
        mt_bat_Crush(create_gen);
    } else if (battery == "BigCrush") {
        mt_bat_BigCrush(create_gen);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    // (10, 7), (17, 5), (55, 24)
    //LFibGenerator<17, 5> lcgobj(123);
//    LcgGenerator lcgobj;
    //bbattery_SmallCrush(lcgobj.GetPtr());
/*
    custom_Crush([] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new LFibGenerator<17, 5>(123));
    });
*/

    return 0;
}
