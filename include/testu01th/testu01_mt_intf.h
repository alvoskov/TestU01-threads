#ifndef __TESTU01_MT_INTF_H
#define __TESTU01_MT_INTF_H

extern "C" {
#include "unif01.h"
#include "gdef.h"
#ifndef PACKAGE_STRING
#include "config.h"
#endif
#include "bbattery.h"
#include "fbar.h"
#include "gofw.h"
#include "gofs.h"
#include "smultin.h"
#include "sknuth.h"
#include "smarsa.h"
#include "snpair.h"
#include "svaria.h"
#include "sstring.h"
#include "swalk.h"
#include "scomp.h"
#include "sspectral.h"
#include "swrite.h"
#include "sres.h"
#include "num.h"
#include "unif01.h"
#include "ufile.h"
}

#define THOUSAND 1000
#define MILLION (THOUSAND * THOUSAND)
#define BILLION (THOUSAND * MILLION)


#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

namespace testu01_threads {

/**
 * @brief Object-oriented envelope for TestU01 structures.
 * Allows to use RAII paradigm instead of manual calloc/free.
 */
class UniformGenerator
{
    static void WrExternGen(void *junk2) { (void) junk2; }
    std::string name;    

    UniformGenerator(const UniformGenerator &obj) = delete;
    UniformGenerator &operator=(const UniformGenerator &obj) = delete;
    static double GetU01Handle(void *param, void *state);
    static unsigned long GetBits32(void *param, void *state);

protected:
    unif01_Gen gen;
    
public:
    UniformGenerator(const std::string &name);
    virtual ~UniformGenerator() {}
    unif01_Gen *GetPtr() const { return const_cast<unif01_Gen *>(&gen); }
    const std::string &GetName() { return name; }
    virtual double GetU01() = 0;
    virtual uint32_t GetBits32() = 0;
    virtual uint64_t GetBits64();
    virtual void GetArray32(uint32_t *out, size_t len);
    virtual void GetArray64(uint64_t *out, size_t len);
    virtual uint32_t GetSum32(size_t len);
    virtual uint64_t GetSum64(size_t len);
};

} // namespace testu01_threads

#endif
