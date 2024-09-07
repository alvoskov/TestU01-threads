#include "pdiehard.h"

using namespace testu01_threads;

PseudoDiehardBattery::PseudoDiehardBattery(GenFactoryFunc genf)
    : TestsBattery(genf)
{
    int j2 = 0;
    battery_name = "pseudoDIEHARD(mt)";

    auto BirthdaySpacings_func = [] (TestDescr &td, BatteryIO &io) {
        long Count[7];
        double NumExp[7] = {
            67.668, 135.335, 135.335, 90.224, 45.112, 18.045, 8.282
        };
        sres_Chi2 *Chi = sres_CreateChi2();
        sres_InitChi2(Chi, 1, 6, (char *) "");
        sres_Poisson *res = sres_CreatePoisson();
        printf("smarsa_BirthdaySpacings test with r = 0, 1, 2, 3, 4, 5,"
            " 6, 7, 8,\n .....\n\n");
        for (int i = 0; i <= 8; i++) {
            printf(" r = %d\n", i);
            for (int k = 0; k <= 6; k++)
                Count[k] = 0;
            for (int k = 0; k < 500; k++) {
                smarsa_BirthdaySpacings(io.Gen(), res, 1, 512, i, 16777216, 1, 1);
                if (res->sVal2 >= 6)
                    ++Count[6];
                else
                    ++Count[(int) res->sVal2];
            }
            double x = gofs_Chi2(NumExp, Count, 0, 6);
            printf ("ChiSquare statistic                   :");
            double pvalue = fbar_ChiSquare2(6, 12, x);
            gofw_Writep2(x, pvalue);
            io.Add(td.GetId(), td.GetName(), pvalue);
        }
        printf ("\n\n\n\n");
        sres_DeletePoisson(res);
        sres_DeleteChi2(Chi);
    };
    tests.emplace_back(++j2, "BirthdaySpacings", BirthdaySpacings_func);

    tests.emplace_back(++j2, "MatrixRank",
        smarsa_MatrixRank_cb(1, 40000, 0, 31, 31, 31));

    tests.emplace_back(++j2, "MatrixRank",
        smarsa_MatrixRank_cb(1, 40000, 0, 32, 32, 32));

    ++j2;
    for (int i = 0; i < 24; i++) {
        tests.emplace_back(j2, "MatrixRank ",
            smarsa_MatrixRank_cb(1, 100000, i, 8, 6, 8));
    }


    tests.emplace_back(++j2, "MultinomialBitsOver", [] (TestDescr &td, BatteryIO &io) {
        double ValDelta[] = { 1 };
        smultin_Param *par = smultin_CreateParam (1, ValDelta, smultin_GenerCellSerial, 0);
        smultin_Res *res = smultin_CreateRes (par);
        smultin_MultinomialBitsOver(io.Gen(), par, res, 20, 2097152, 0, 32, 20, TRUE);
        io.Add(td.GetId(), td.GetName(), res->pVal2[0][gofw_AD]);
        smultin_DeleteRes (res);
        smultin_DeleteParam (par);
    });

    ++j2;
    for (int i = 22; i >= 0; i--) {
        tests.emplace_back(j2, "OPSO", smarsa_Opso_cb(1, i, 1));
    }

    ++j2;
    for (int i = 27; i >= 0; i--) {
        tests.emplace_back(j2, "OQSO", smarsa_Oqso_cb(i));
    }

    ++j2;
    for (int i = 30; i >= 0; i--) {
        tests.emplace_back(j2, "DNA", smarsa_Dna_cb(i));
    }

    j2 += 2;

    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairsNP_cb(100, 8000, 0, 2, 2, 1));

    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairsNP_cb(20, 4000, 0, 3, 2, 1));

    tests.emplace_back(++j2, "Savir2",
        smarsa_Savir2_cb(1, 100000, 0, 90000, 18));

    tests.emplace_back(++j2, "Run of U01",
        sknuth_Run_cb(10, 10000, 0, true));

    tests.emplace_back(++j2, "Run of U01",
        sknuth_Run_cb(10, 10000, 0, false));

    tests.emplace_back(++j2, "Run of U01",
        sknuth_Run_cb(10, 10000, 0, true));

    tests.emplace_back(++j2, "Run of U01",
        sknuth_Run_cb(10, 10000, 0, false));
}
