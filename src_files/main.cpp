
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Tuning.h"
#include "Verification.h"
#include "uci.h"

#include <iomanip>

using namespace std;
using namespace bb;
using namespace move;

void main_tune_features() {
    psqt_init();
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

//    loadPositionFile("../resources/other/quiet-labeled2.epd", 100000);
    loadPositionFile("../resources/other/E12.33-1M-D12-Resolved.book", 1000000);
    std::cout << computeError(3) << std::endl;
    loadPositionFile("../resources/other/E12.41-1M-D12-Resolved.book", 10000000);
    loadPositionFile("../resources/other/E12.46FRC-1250k-D12-1s-Resolved.book", 10000000);
//    auto K = tuning::computeK(2.86681, 200, 1e-7, 24);
    double K = 2.5;
    
    for (int i = 0; i < 100; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;
        
            
        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_square_table[0][0]),64,10,3,24);
        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_square_table[0][1]),64,10,3,24);
        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_our_king_square_table[0]),225,10,3,24);
        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_opp_king_square_table[0]),225,10,3,24);
        
        // std::cout << tuning::optimisePSTBlackBox(evaluator, K, pieceScores, 6, 1) << std::endl;
        // std::cout << tuning::optimisePSTBlackBox(evaluator, K, &evfeatures[21], 2, 1) << std::endl;
        // std::cout << tuning::optimisePSTBlackBox(evaluator, K, pinnedEval, 15, 1) << std::endl;
        // std::cout << tuning::optimisePSTBlackBox(evaluator, K, hangingEval, 5, 1) << std::endl;
//        std::cout << tuning::optimisePSTBlackBox(K, &piece_square_table[0][0], 225, 100, 3, 24) << std::endl;
//
        for (int s = 0; s < 64; s++) {
            if (s%8 == 0) std::cout << std::endl;
            std::cout << "M(" << setw(5) << MgScore(piece_square_table[0][0][s]) << "," << setw(5)
                      << EgScore(piece_square_table[0][0][s]) << "), ";
           
        }
        std::cout << std::endl;
        for (int s = 0; s < 64; s++) {
            if (s%8 == 0) std::cout << std::endl;
            std::cout << "M(" << setw(5) << MgScore(piece_square_table[0][1][s]) << "," << setw(5)
                      << EgScore(piece_square_table[0][1][s]) << "), ";
        }
        std::cout << std::endl;
        for (int s = 0; s < 225; s++) {
            if (s%15 == 0) std::cout << std::endl;
            std::cout << "M(" << setw(5) << MgScore(piece_our_king_square_table[0][s]) << "," << setw(5)
                      << EgScore(piece_our_king_square_table[0][s]) << "), ";
        }
        std::cout << std::endl;
        for (int s = 0; s < 225; s++) {
            if (s%15 == 0) std::cout << std::endl;
            std::cout << "M(" << setw(5) << MgScore(piece_opp_king_square_table[0][s]) << "," << setw(5)
                      << EgScore(piece_opp_king_square_table[0][s]) << "), ";
        }
        std::cout << std::endl;
//        std::cout << std::endl;

        /*for (Square s = 0; s < 23; s++) {
            std::cout << "M(" << setw(5) << MgScore(*evfeatures[s]) << "," << setw(5) << EgScore(*evfeatures[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
*/
        /*for (Square s = 0; s < 15; s++) {
            std::cout << "M(" << setw(5) << MgScore(pinnedEval[s]) << "," << setw(5) << EgScore(pinnedEval[s]) << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 5; s++) {
            std::cout << "M(" << setw(5) << MgScore(hangingEval[s]) << "," << setw(5) << EgScore(hangingEval[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;*/
    }

    delete evaluator;
    bb_cleanUp();
}

int main(int argc, char* argv[]) {
    
//    bb_init();
//    psqt_init();
//
//    Board b{"8/2n2k2/1q1r3p/p1b1pPP1/1RPQ1B2/3K1n2/4N2R/8 w - - 0 1"};
//    Evaluator evaluator{};
//    std::cout << evaluator.evaluate(&b)<<std::endl;
//    exit(-1);
//
//    bb_init();
//    psqt_init();
//
//    Board b{"1k6/8/3p4/8/8/3P4/8/K7 w - - 0 1"};
//    Evaluator evaluator{};
//    std::cout << evaluator.evaluate(&b)<<std::endl;
    
    /**********************************************************************************
     *                                  T U N I N G                                   *
     **********************************************************************************/

    // main_tune_pst_bb(PAWN);
    //    psqt_init();
     main_tune_features();
    // main_tune_pst();
    // main_tune_features_bb();

    return 0;
}
