#include "testu01_mt.h"

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

//////////////////////////
///// Some functions /////
//////////////////////////


static void thread_func(std::vector<TestDescr> &tests, BatteryIO &io, int thread_id)
{
    size_t ntests = tests.size(), i = 1;
    printf("vvvvvvvvvv  Thread #%d started  vvvvvvvvvv\n", thread_id);
    for (auto &t : tests) {
        printf("vvvvv  Thread #%d: test %s started (test %d of %d)\n",
            thread_id, t.GetName().c_str(), (int) i, (int) ntests);
        t.Run(io);
        printf("^^^^^  Thread #%d: test %s finished (test %d of %d)\n",
            thread_id, t.GetName().c_str(), (int) i++, (int) ntests);
    }
    printf("^^^^^^^^^^  Thread #%d finished  ^^^^^^^^^^\n", thread_id);
}

void run_tests(std::vector<TestDescr> &tests,
    std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string &battery_name)
{
    chrono_Chrono *timer = chrono_Create();

    size_t nthreads = std::thread::hardware_concurrency();
    size_t ntests = tests.size();
    while (nthreads > ntests)
        nthreads /= 2;
    printf("=====> Number of threads: %d\n", (int) nthreads);
    std::vector<std::vector<TestDescr>> threads_tasks(nthreads);
    std::vector<BatteryIO> threads_bats;
    for (size_t i = 0; i < nthreads; i++) {
        threads_bats.emplace_back(create_gen());
    }
    size_t th_id = 0;
    for (auto &t : tests) {
        threads_tasks[th_id].push_back(t);
        if (++th_id == nthreads)
            th_id = 0;
    }

    std::vector<std::thread> threads;
    for (size_t i = 0; i < nthreads; i++) {
        threads.emplace_back(thread_func,
            std::ref(threads_tasks[i]),
            std::ref(threads_bats[i]),
            i);
    }

    for (auto &th : threads) {
        th.join();
    }

    auto gen = create_gen();
    BatteryIO io(gen);
    for (auto &bat : threads_bats) {
        io.Add(bat);
    }
    io.WriteReport(battery_name.c_str(), gen.get()->GetName().c_str(), timer);
    chrono_Delete(timer);
}


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


TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        auto *res2 = sres_CreateChi2 ();
        sknuth_CouponCollector (io.Gen(), res2, N, n, r, d);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
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
        io.Add(td.GetId(), td.GetName(), res->GCD->pVal2[gofw_Mean]);
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
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        sstring_DeleteRes (res);
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
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc sknuth_MaxOft_cb(long N, long n, int r, int d, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        auto *res5 = sknuth_CreateRes1 ();
        sknuth_MaxOft (io.Gen(), res5, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res5->Chi->pVal2[gofw_Mean]);
        std::string ad_name = td.GetName();
        ad_name.replace(ad_name.find("MaxOft"), sizeof("MaxOft") - 1, "MaxOft AD");
        io.Add(td.GetId(), ad_name, res5->Bas->pVal2[gofw_Mean]);
        sknuth_DeleteRes1(res5);        
    };
}


TestCbFunc sstring_PeriodsInStrings_cb(long N, long n, int r, int s)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sstring_PeriodsInStrings(io.Gen(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
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


TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t)
{
    return [=] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd(io.Gen(), res, N, n, r, t);
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
