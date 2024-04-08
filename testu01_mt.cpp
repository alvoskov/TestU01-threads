#include "testu01_mt.h"
#include <chrono>
#include <random>
#include <fcntl.h>

/////////////////////////////////////////////////
///// UniformGenerator class implementation /////
/////////////////////////////////////////////////

double UniformGenerator::GetU01Handle(void *param, void *state)
{
    (void) param;
    UniformGenerator *obj = static_cast<UniformGenerator *>(state);
    return obj->GetU01();
}

unsigned long UniformGenerator::GetBits(void *param, void *state)
{
    (void) param;
    UniformGenerator *obj = static_cast<UniformGenerator *>(state);
    return obj->GetBits();
}

/**
 * @brief Returns 64 bits from PRNG. Not used by TestU01, reserved
 * mainly for future use and testing 64-bit generators by PractRand.
 */
uint64_t UniformGenerator::GetBits64()
{
    return 0;
}


UniformGenerator::UniformGenerator(const std::string &name)
{
    this->name = name;
    gen.state = static_cast<void *>(this);
    gen.param = nullptr;
    gen.Write = WrExternGen;
    gen.GetU01 = GetU01Handle;
    gen.GetBits = GetBits;
    gen.name = const_cast<char *>(name.c_str());
}


//////////////////////////////////////////////////
///// UniformGeneratorC class implementation /////
//////////////////////////////////////////////////


UniformGeneratorC::UniformGeneratorC(const GenInfoC *gi)
: UniformGenerator("")
{
    this->name = std::string(gi->name);
    gen.state = gi->init_state();
    gen.param = nullptr;
    gen.Write = WrExternGen;
    gen.GetU01 = gi->get_u01;
    gen.GetBits = gi->get_bits32;
    gen.name = const_cast<char *>(name.c_str());
    get_bits64 = gi->get_bits64;
    destroy = gi->delete_state;
}

//////////////////////////////////////////
///// BatteryIO class implementation /////
//////////////////////////////////////////


size_t BatteryIO::GetNTestsFailed() const
{
    size_t co = 0;
    for (auto &r : results) {
        if ((r.pvalue < gofw_Suspectp) || (r.pvalue > 1.0 - gofw_Suspectp)) {
            co++;
        }
    }
    return co;
}

/*
 * Write a p-value with a nice format.
 */
void BatteryIO::WritePValue (double p)
{
    if (p < gofw_Suspectp) {
        gofw_Writep0 (p);
    } else if (p > 1.0 - gofw_Suspectp) {
        if (p >= 1.0 - gofw_Epsilonp1) {
            printf (" 1 - eps1");
        } else if (p >= 1.0 - 1.0e-4) {
            printf (" 1 - ");
            num_WriteD (1.0 - p, 7, 2, 2);
        } else if (p >= 1.0 - 1.0e-2) {
            printf ("  %.4f ", p);
        } else {
            printf ("   %.2f", p);
        }
    }
}

void BatteryIO::Add(const BatteryIO &obj)
{
    for (auto &o : obj.results) {
        results.push_back(o);
    }
    std::sort(results.begin(), results.end());
}

void BatteryIO::WriteReport(const char *batName, const char *genName, chrono_Chrono *timer)
{
    printf("\n========= Summary results of %s", batName);
    printf(" =========\n\n");
    printf(" Version:          %s\n", PACKAGE_STRING);
    printf(" Generator:        "); printf ("%s", genName);


    printf("\n Number of statistics:  %1d\n", (int) results.size());
    //for (auto &r : results) {
    //    printf("-->%d %s\n", r.id, r.name.c_str());
    //}
    printf(" Total CPU time:   ");
    chrono_Write(timer, chrono_hms);

    if (GetNTestsFailed() == 0) {
        printf ("\n\n All tests were passed\n\n\n\n");
        return;
    }

    if (gofw_Suspectp >= 0.01)
        printf ("\n The following tests gave p-values outside [%.4g, %.2f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    else if (gofw_Suspectp >= 0.0001)
        printf ("\n The following tests gave p-values outside [%.4g, %.4f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    else if (gofw_Suspectp >= 0.000001)
        printf ("\n The following tests gave p-values outside [%.4g, %.6f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    else
        printf ("\n The following tests gave p-values outside [%.4g, %.14f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    printf (":\n (eps  means a value < %6.1e)", gofw_Epsilonp);
    printf (":\n (eps1 means a value < %6.1e)", gofw_Epsilonp1);
    printf (":\n\n       Test                          p-value\n");
    printf (" ----------------------------------------------\n");

    for (auto &r : results) {
        if ((r.pvalue >= gofw_Suspectp) && (r.pvalue <= 1.0 - gofw_Suspectp))
            continue; // That test was passed
        printf(" %2d ", r.id);
        printf(" %-30s", r.name.c_str());
        WritePValue(r.pvalue);
        printf("\n");
    }

    printf (" ----------------------------------------------\n");
    printf (" All other tests were passed\n");
    printf ("\n\n\n");
}


//////////////////////////////////////////
///// TestsPull class implementation /////
//////////////////////////////////////////

TestsPull::TestsPull(const std::vector<TestDescr> &obj)
{
    pos = 0;
    size_t len = obj.size();
    std::vector<size_t> tests_inds(len);
    for (size_t i = 0; i < len; i++) {
        tests_inds[i] = i;
    }
    std::random_device rd;
    std::mt19937 prng(rd()); 
    std::shuffle(tests_inds.begin(), tests_inds.end(), prng);

    for (auto ind : tests_inds) {
        tests.push_back(obj[ind]);
    }
}

/**
 * @brief Returns the pointer to the next test that was not processed.
 * @param[out] pos_msg Buffer for the `test _ of _` message.
 * @return Pointer to the test (success) or nullptr (failure, i.e.
 * no more tests left).
 */
const TestDescr *TestsPull::Get(std::string &pos_msg)
{
    std::lock_guard<std::mutex> lock(get_mutex);
    if (pos < tests.size()) {
        pos_msg = "test " + std::to_string(pos + 1) +
            " of " + std::to_string(tests.size());
        return &tests[pos++];
    } else {
        pos_msg = "NONE";
        return nullptr;
    }
}


size_t TestsPull::GetNThreads() const
{
    size_t nthreads = std::thread::hardware_concurrency();
    size_t ntests = tests.size();
    while (nthreads > ntests)
        nthreads /= 2;
    return nthreads;
}


void TestsPull::ThreadFunc(TestsPull &pull, BatteryIO &io, int thread_id)
{
    fprintf(stderr, "vvvvvvvvvv  Thread #%d started  vvvvvvvvvv\n", thread_id);
    const TestDescr *test = nullptr;
    std::string pos_msg;
    while ((test = pull.Get(pos_msg)) != nullptr) {
        TestDescr t = *test;
        fprintf(stderr, "vvvvv  Thread #%d: test %s started (%s)\n",
            thread_id, t.GetName().c_str(), pos_msg.c_str());
        size_t ind1 = io.GetNResults();
        t.Run(io);
        size_t ind2 = io.GetNResults();
        fprintf(stderr, "^^^^^  Thread #%d: test %s finished (%s)",
            thread_id, t.GetName().c_str(), pos_msg.c_str());
        if (ind2 > ind1) {
            ind2--;
            fprintf(stderr, "; p = [");
            for (size_t i = ind1; i <= ind2; i++) {
                fprintf(stderr, "%g ", io.GetPValueRecord(i).pvalue);
            }
            fprintf(stderr, "]\n");
        } else {
            fprintf(stderr, "\n");
        }
        

    }
    fprintf(stderr, "^^^^^^^^^^  Thread #%d finished  ^^^^^^^^^^\n", thread_id);
}


void TestsPull::Run(std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string &battery_name)
{
    // Timers and threads number
    chrono_Chrono *timer = chrono_Create();
    size_t nthreads = GetNThreads();
    fprintf(stderr, "=====> Number of threads: %d\n", (int) nthreads);
    std::vector<BatteryIO> threads_bats;
    for (size_t i = 0; i < nthreads; i++) {
        threads_bats.emplace_back(create_gen());
    }
    // Disable thread unsafe features of TestU01
    swrite_Host = FALSE;
    // Multi-threaded run
    auto tic = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    for (size_t i = 0; i < nthreads; i++) {
        threads.emplace_back(ThreadFunc,
            std::ref(*this),
            std::ref(threads_bats[i]),
            i);
    }
    for (auto &th : threads) {
        th.join();
    }
    // Merge results from different threads
    auto gen = create_gen();
    BatteryIO io(gen);
    for (auto &bat : threads_bats) {
        io.Add(bat);
    }
    // Estimate the elapsed time
    auto toc = std::chrono::high_resolution_clock::now();    
    size_t ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count();
    size_t ms = ms_total % 1000;
    size_t s = (ms_total / 1000) % 60;
    size_t m = (ms_total / 60000) % 60;
    size_t h = (ms_total / 3600000);
    fprintf(stderr, "=====> Elapsed time: %.2d:%.2d:%.2d.%.3d\n",
        (int) h, (int) m, (int) s, (int) ms);
    // Print report
    io.WriteReport(battery_name.c_str(), gen.get()->GetName().c_str(), timer);
    chrono_Delete(timer);
}

/////////////////////////////////////////////
///// TestsBattery class implementation /////
/////////////////////////////////////////////



TestsBattery::TestsBattery(GenFactoryFunc genf)
    : create_gen(genf)
{
}

void TestsBattery::Run() const
{
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting %s\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        battery_name.c_str(), PACKAGE_STRING);

    TestsPull pull(tests);
    pull.Run(create_gen, battery_name);
}

bool TestsBattery::RunTest(int id) const
{
    if (id <= 0) {
        Run();
        return true;
    }

    std::vector<TestDescr> t;
    for (size_t i = 0; i < tests.size(); i++) {
        if (tests[i].GetId() == id)
            t.push_back(tests[i]);
    }
    if (t.size() == 0)
        return false;

    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting %s test %d\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        battery_name.c_str(), id, PACKAGE_STRING);

    TestsPull pull(t);
    pull.Run(create_gen, battery_name + " test " + std::to_string(id));
    return true;
}



//////////////////////////
///// Some functions /////
//////////////////////////

/**
 * @brief Get the p-values in a swalk_RandomWalk1 test
 * @details It is a rewrite of the `GetPVal_Walk` function from `bbattery.c`.
 */
static void GetPValue_Walk(BatteryIO &io, long N, swalk_Res *res, size_t id, const std::string &mess)
{
   if (N == 1) {
        io.Add(id, "RandomWalk1 H" + mess, res->H[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 M" + mess, res->M[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 J" + mess, res->J[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 R" + mess, res->R[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 C" + mess, res->C[0]->pVal2[gofw_Mean]);
   } else {
        io.Add(id, "RandomWalk1 H" + mess, res->H[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 M" + mess, res->M[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 J" + mess, res->J[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 R" + mess, res->R[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 C" + mess, res->C[0]->pVal2[gofw_Sum]);
   }
}

/**
 * @brief Get the p-values in a snpair_ClosePairs test
 * @param flag Former snpair_mNP2S_Flag global variable (made local for thread safety).
 */
static void GetPValue_CPairs(BatteryIO &io, long N, snpair_Res *res, size_t id, const std::string &mess, bool flag)
{
    if (N == 1) {
        io.Add(id, "ClosePairs NP" + mess, res->pVal[snpair_NP]);
        io.Add(id, "ClosePairs mNP" + mess, res->pVal[snpair_mNP]);
   } else {
        io.Add(id, "ClosePairs NP" + mess, res->pVal[snpair_NP]);
        io.Add(id, "ClosePairs mNP" + mess, res->pVal[snpair_mNP]);
        io.Add(id, "ClosePairs mNP1" + mess, res->pVal[snpair_mNP1]);
        io.Add(id, "ClosePairs mNP2" + mess, res->pVal[snpair_mNP2]);
        io.Add(id, "ClosePairs NJumps" + mess, res->pVal[snpair_NJumps]);
        if (flag) {
            io.Add(id, "ClosePairs mNP2S" + mess, res->pVal[snpair_mNP2S]);
        }
   }
}


void set_bin_stdout()
{
#ifdef USE_LOADLIBRARY
    _setmode( _fileno(stdout), _O_BINARY); // needed to allow binary stdout on windows
#endif
}


/**
 * @brief Dump an output of a 32-bit PRNG to the stdout in the format suitable
 * for PractRand.
 */
void prng_bits32_to_file(std::shared_ptr<UniformGenerator> genptr)
{
    uint32_t buf[256];
    set_bin_stdout();
    while (1) {
        for (int i = 0; i < 256; i++) {
            buf[i] = genptr->GetBits();
        }
        fwrite(buf, sizeof(uint32_t), 256, stdout);
    }
}

/**
 * @brief Dump an output of a 64-bit PRNG to the stdout in the format suitable
 * for PractRand.
 */
void prng_bits64_to_file(std::shared_ptr<UniformGenerator> genptr)
{
    uint64_t buf[256];
    set_bin_stdout();
    while (1) {
        for (int i = 0; i < 256; i++) {
            buf[i] = genptr->GetBits64();
        }
        fwrite(buf, sizeof(uint64_t), 256, stdout);
    }
}



///////////////////////////////////////////////////////
///// Functions that generate callbacks for tests /////
///////////////////////////////////////////////////////


TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_AppearanceSpacings(io.Gen(), res, N, Q, K, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        sstring_AutoCor(io.Gen(), res, N, n, r, s, d);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Poisson *res = sres_CreatePoisson();
        smarsa_BirthdaySpacings(io.Gen(), res, N, n, r, d, t, p);
        io.Add(td.GetId(), td.GetName(), res->pVal2);
        sres_DeletePoisson(res);
    };
}

TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        smarsa_Res *res = smarsa_CreateRes();
        smarsa_CollisionOver (io.Gen(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sknuth_Res2 *res = sknuth_CreateRes2 ();
        sknuth_CollisionPermut(io.Gen(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        sknuth_DeleteRes2 (res);
    };
}

TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        auto *res2 = sres_CreateChi2 ();
        sknuth_CouponCollector (io.Gen(), res2, N, n, r, d);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    };
}


TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m, const std::string &mess, bool flag)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        snpair_Res *res = snpair_CreateRes();
        snpair_ClosePairs(io.Gen(), res, N, n, r, k, p, m);
        GetPValue_CPairs(io, 10, res, td.GetId(), mess, flag);
        snpair_DeleteRes(res);
    };
}

/**
 * @brief Needed for pseudoDIEHARD battery.
 */
TestCbFunc snpair_ClosePairsNP_cb(long N, long n, int r, int k, int p, int m)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        snpair_Res *res = snpair_CreateRes();
        snpair_ClosePairs(io.Gen(), res, N, n, r, k, p, m);
        io.Add(td.GetId(), td.GetName(), res->pVal[snpair_NP]);
        snpair_DeleteRes(res);
    };
}

TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        snpair_Res *res = snpair_CreateRes();
        snpair_ClosePairsBitMatch(io.Gen(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->pVal[snpair_BM]);
        snpair_DeleteRes(res);
    };
}

/**
 * @brief An envelope for smarsa_CollisionOver for pseudoDIEHARD battery.
 */
TestCbFunc smarsa_Dna_cb(int i)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        printf ("***********************************************************\n"
            "Test DNA calling smarsa_CollisionOver\n\n");
        smarsa_Res *res = smarsa_CreateRes();
        smarsa_CollisionOver(io.Gen(), res, 1, 2097152, i, 4, 10);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sspectral_Fourier3_cb(long N, int k, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sspectral_Res *res = sspectral_CreateRes();
        sspectral_Fourier3(io.Gen(), res, N, k, r, s);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_AD]);
        sspectral_DeleteRes(res);
    };
}


TestCbFunc sknuth_Gap_cb(long N, long n, int r, double Alpha, double Beta)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Gap(io.Gen(), res, N, n, r, Alpha, Beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_GCD_cb(long N, long n, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        smarsa_Res2 *res = smarsa_CreateRes2();
        smarsa_GCD (io.Gen(), res, N, n, r, s);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->GCD->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->GCD->pVal2[gofw_Sum]);
        smarsa_DeleteRes2(res);
    };
}


TestCbFunc sstring_HammingCorr_cb(long N, long n, int r, int s, int L)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sstring_Res *res = sstring_CreateRes();
        sstring_HammingCorr(io.Gen(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        sstring_DeleteRes(res);

    };
}

TestCbFunc sstring_HammingIndep_cb(long N, long n, int r, int s, int L, int d)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sstring_Res *res = sstring_CreateRes();
        sstring_HammingIndep(io.Gen(), res, N, n, r, s, L, d);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Sum]);
        sstring_DeleteRes(res);
    };
}

TestCbFunc sstring_HammingWeight2_cb(long N, int r, int s, long L, long K)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        sstring_HammingWeight2(io.Gen(), res, N, r, s, L, K);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc scomp_LempelZiv_cb(long N, int t, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        scomp_LempelZiv(io.Gen(), res, N, t, r, s);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}


TestCbFunc scomp_LinearComp_cb(long N, long n, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        scomp_Res *res = scomp_CreateRes();
        scomp_LinearComp(io.Gen(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->JumpNum->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->JumpSize->pVal2[gofw_Mean]);
        scomp_DeleteRes(res);
    };
}

TestCbFunc sstring_LongestHeadRun_cb(long N, long n, int r, int s, long L)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sstring_Res2 *res = sstring_CreateRes2();
        sstring_LongestHeadRun(io.Gen(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Chi->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->Disc->pVal2);
        sstring_DeleteRes2(res);
    };
}


TestCbFunc smarsa_MatrixRank_cb(long N, long n, int r, int s, int L, int k)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        smarsa_MatrixRank(io.Gen(), res, N, n, r, s, L, k);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc sknuth_MaxOft_cb(long N, long n, int r, int d, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        gofw_TestType type_chi = gofw_Sum, type_bas = gofw_AD;
        if (N == 1) {
            type_chi = gofw_Mean;
            type_bas = gofw_Mean;
        }
        auto *res5 = sknuth_CreateRes1 ();
        sknuth_MaxOft (io.Gen(), res5, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res5->Chi->pVal2[type_chi]);
        std::string ad_name = td.GetName();
        ad_name.replace(ad_name.find("MaxOft"), sizeof("MaxOft") - 1, "MaxOft AD");
        io.Add(td.GetId(), ad_name, res5->Bas->pVal2[type_bas]);
        sknuth_DeleteRes1(res5);        
    };
}


TestCbFunc smarsa_Opso_cb(long N, int r, int p)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        smarsa_Res *res = smarsa_CreateRes();
        smarsa_Opso(io.Gen(), res, N, r, p);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        smarsa_DeleteRes(res);
    };
}

/**
 * @brief An envelope for smarsa_CollisionOver for pseudoDIEHARD battery.
 */
TestCbFunc smarsa_Oqso_cb(int i)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        printf ("***********************************************************\n"
            "Test OQSO calling smarsa_CollisionOver\n\n");
        smarsa_Res *res = smarsa_CreateRes();
        smarsa_CollisionOver(io.Gen(), res, 1, 2097152, i, 32, 4);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}



TestCbFunc sstring_PeriodsInStrings_cb(long N, long n, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sstring_PeriodsInStrings(io.Gen(), res, N, n, r, s);
        if (N == 1)        
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2 (res);
    };
}

TestCbFunc sknuth_Permutation_cb(long N, long n, int r, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Permutation(io.Gen(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_RandomWalk1_cb(long N, long n, int r, int s,
    long L0, long L1, const std::string &mess)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        auto *res = swalk_CreateRes ();
        swalk_RandomWalk1 (io.Gen(), res, N, n, r, s, L0, L1);
        GetPValue_Walk(io, 1, res, td.GetId(), mess.c_str());
        swalk_DeleteRes(res);
    };
}

TestCbFunc sknuth_Run_cb(long N, long n, int r, bool Up)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2 ();
        sknuth_Run(io.Gen(), res, N, n, r, Up);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc sstring_Run_cb(long N, long n, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sstring_Res3 *res = sstring_CreateRes3();
        sstring_Run(io.Gen(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->NRuns->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->NBits->pVal2[gofw_Mean]);
        sstring_DeleteRes3 (res);
    };
}

TestCbFunc svaria_SampleCorr_cb(long N, long n, int r, int k)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleCorr(io.Gen(), res, N, n, r, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd(io.Gen(), res, N, n, r, t);
        if (N > 1) // Derived from comparison of Crush and BigCrush
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc svaria_SampleMean_cb(long N, long n, int r)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleMean(io.Gen(), res, N, n, r);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_Savir2_cb(long N, long n, int r, long m, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        auto *res = sres_CreateChi2();
        smarsa_Savir2(io.Gen(), res, N, n, r, m, t);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc smarsa_SerialOver_cb(long N, long n, int r, long d, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        smarsa_SerialOver(io.Gen(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc sknuth_SimpPoker_cb(long N, long n, int r, int d, int k)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, N, n, r, d, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_SumCollector_cb(long N, long n, int r, double g)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        svaria_SumCollector(io.Gen(), res, N, n, r, g);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_WeightDistrib_cb(long N, long n, int r, long k,
    double alpha, double beta)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        svaria_WeightDistrib(io.Gen(), res, N, n, r, k, alpha, beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}
