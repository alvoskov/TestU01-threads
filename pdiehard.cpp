#include "pdiehard.h"


PseudoDiehardBattery::PseudoDiehardBattery(GenFactoryFunc genf)
    : TestsBattery(genf)
{
//    const int r = 0;
    int j2 = 0;
    battery_name = "pseudoDIEHARD(mt)";

//   chrono_Chrono *Timer;
//   smultin_Param *par = NULL;
//   double ValDelta[] = { 1 };
//   int k, i, j = -1;
//   double x;
//   long Count[7];
//   double NumExp[7] = {
//      67.668, 135.335, 135.335, 90.224, 45.112, 18.045, 8.282
//   };



/*
   {
      sres_Poisson *res;
      sres_Chi2 *Chi;
      Chi = sres_CreateChi2 ();
      sres_InitChi2 (Chi, 1, 6, "");
      res = sres_CreatePoisson ();
      printf ("smarsa_BirthdaySpacings test with r = 0, 1, 2, 3, 4, 5,"
         " 6, 7, 8,\n .....\n\n");
      swrite_Basic = FALSE;
      ++j2;
      for (i = 0; i <= 8; i++) {
         printf (" r = %d\n", i);
         for (k = 0; k <= 6; k++)
            Count[k] = 0;
         for (k = 0; k < 500; k++) {
            smarsa_BirthdaySpacings (gen, res, 1, 512, i, 16777216, 1, 1);
            if (res->sVal2 >= 6)
               ++Count[6];
            else
               ++Count[(int) res->sVal2];
         }
         x = gofs_Chi2 (NumExp, Count, 0, 6);
         printf ("ChiSquare statistic                   :");
         bbattery_pVal[++j] = fbar_ChiSquare2 (6, 12, x);
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "BirthdaySpacings");
         gofw_Writep2 (x, bbattery_pVal[j]);
      }
      printf ("\n\n\n\n");
      sres_DeletePoisson (res);
      sres_DeleteChi2 (Chi);
      swrite_Basic = TRUE;
   }
*/


    tests.emplace_back(++j2, "MatrixRank",
        smarsa_MatrixRank_cb(1, 40000, 0, 31, 31, 31));

    tests.emplace_back(++j2, "MatrixRank",
        smarsa_MatrixRank_cb(1, 40000, 0, 32, 32, 32));

    ++j2;
    for (int i = 0; i < 24; i++) {
        tests.emplace_back(j2, "MatrixRank ",
            smarsa_MatrixRank_cb(1, 100000, i, 8, 6, 8));
    }
        
/*
   {
      smultin_Res *res;
      par = smultin_CreateParam (1, ValDelta, smultin_GenerCellSerial, 0);
      res = smultin_CreateRes (par);
      smultin_MultinomialBitsOver (gen, par, res, 20, 2097152, 0, 32, 20,
         TRUE);
      bbattery_pVal[++j] = res->pVal2[0][gofw_AD];
      TestNumber[j] = ++j2;
      strcpy (bbattery_TestNames[j], "MultinomialBitsOver");
      smultin_DeleteRes (res);
      smultin_DeleteParam (par);
   }
*/

/*
   {
      smarsa_Res *res;
      res = smarsa_CreateRes ();
      ++j2;
      for (i = 22; i >= 0; i--) {
         smarsa_Opso (gen, res, 1, i, 1);
         bbattery_pVal[++j] = res->Pois->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "OPSO");
      }
      ValDelta[0] = -1.0;
      ++j2;
      for (i = 27; i >= 0; i--) {
         if (swrite_Basic)
            printf ("***********************************************************\n"
               "Test OQSO calling smarsa_CollisionOver\n\n");
         smarsa_CollisionOver (gen, res, 1, 2097152, i, 32, 4);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "OQSO");
      }
      ++j2;
      for (i = 30; i >= 0; i--) {
         if (swrite_Basic)
            printf ("***********************************************************\n"
               "Test DNA calling smarsa_CollisionOver\n\n");
         smarsa_CollisionOver (gen, res, 1, 2097152, i, 4, 10);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "DNA");
      }
      smarsa_DeleteRes (res);
   }
   j2 += 2;
   {
      snpair_Res *res;
      res = snpair_CreateRes ();
      snpair_ClosePairs (gen, res, 100, 8000, 0, 2, 2, 1);
      bbattery_pVal[++j] = res->pVal[snpair_NP];
      TestNumber[j] = ++j2;
      strcpy (bbattery_TestNames[j], "ClosePairs");

      snpair_ClosePairs (gen, res, 20, 4000, 0, 3, 2, 1);
      bbattery_pVal[++j] = res->pVal[snpair_NP];
      TestNumber[j] = ++j2;
      strcpy (bbattery_TestNames[j], "ClosePairs");
      snpair_DeleteRes (res);
   }

    */

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
