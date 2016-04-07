// Name: Xu Chai
// Email: chaix@rpi.edu

// ==========================================================================
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>

#include "MersenneTwister.h"

#include "tile.h"
#include "location.h"
#include "board.h"


// this global variable is set in main.cpp and is adjustable from the command line
// (you are not allowed to make your own global variables)
int GLOBAL_TILE_SIZE = 11;



// ==========================================================================
// Helper function that is called when an error in the command line
// arguments is detected.
void usage(int argc, char *argv[]) {
    std::cerr << "USAGE: " << std::endl;
    std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>" << std::endl;
    std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>  -all_solutions" << std::endl;
    std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>  -allow_rotations" << std::endl;
    std::cerr << "  " << argv[0] << " <filename>  -all_solutions  -allow_rotations" << std::endl;
    std::cerr << "  " << argv[0] << " <filename>  -tile_size <odd # >= 11>" << std::endl;
    exit(1);
}



// ==========================================================================
// To get you started, this function places tiles on the board and
// randomly and outputs the results (in all likelihood *not* a
// solution!) in the required format
void RandomlyPlaceTiles(Board &board, const std::vector<Tile*> &tiles, std::vector<Location> &locations) {
    
    // MersenneTwister is an excellent library for psuedo-random numbers!
    MTRand mtrand;
    
    for (int t = 0; t < tiles.size(); t++) {
        // loop generates random locations until we (eventually) find one
        // that is not occupied
        int i,j;
        do {
            // generate a random coordinate within the range 0,0 -> rows-1,cols-1
            i = mtrand.randInt(board.numRows()-1);
            j = mtrand.randInt(board.numColumns()-1);
        } while (board.getTile(i,j) != NULL);
        
        // rotation is always 0 (for now)
        locations.push_back(Location(i,j,0));
        board.setTile(i,j,tiles[t]);
    }
}


// ==========================================================================
void HandleCommandLineArguments(int argc, char *argv[], std::string &filename,
                                int &rows, int &columns, bool &all_solutions, bool &allow_rotations) {
    
    // must at least put the filename on the command line
    if (argc < 2) {
        usage(argc,argv);
    }
    filename = argv[1];
    
    // parse the optional arguments
    for (int i = 2; i < argc; i++) {
        // change the title_size using command line
        if (argv[i] == std::string("-tile_size")) {
            i++;
            assert (i < argc);
            GLOBAL_TILE_SIZE = atoi(argv[i]);
            if (GLOBAL_TILE_SIZE < 11 || GLOBAL_TILE_SIZE % 2 == 0) {
                std::cerr << "ERROR: bad tile_size" << std::endl;
                usage(argc,argv);
            }
        }
        // if find all solutions or not
        else if (argv[i] == std::string("-all_solutions")) {
            all_solutions = true;
        }
        // setting board dimensions
        else if (argv[i] == std::string("-board_dimensions")) {
            i++;
            assert (i < argc);
            rows = atoi(argv[i]);
            i++;
            assert (i < argc);
            columns = atoi(argv[i]);
            if (rows < 1 || columns < 1) {
                usage(argc,argv);
            }
        }
        // if allow rotations or not
        else if (argv[i] == std::string("-allow_rotations")) {
            allow_rotations = true;
        } else {
            std::cerr << "ERROR: unknown argument '" << argv[i] << "'" << std::endl;
            usage(argc,argv);
        }
    }
}


// ==========================================================================
void ParseInputFile(int argc, char *argv[], const std::string &filename, std::vector<Tile*> &tiles) {
    
    // open the file
    std::ifstream istr(filename.c_str());
    if (!istr) {
        std::cerr << "ERROR: cannot open file '" << filename << "'" << std::endl;
        usage(argc,argv);
    }
    assert (istr);
    
    // read each line of the file
    std::string token, north, east, south, west;
    while (istr >> token >> north >> east >> south >> west) {
        assert (token == "tile");
        Tile *t = new Tile(north,east,south,west);
        tiles.push_back(t);
    }
}

//---------------------------------------------------------------------------------------
// This function is used for checking the whole layout of the board after all the tiles have been used up.
bool Check_the_whole_board(const Board &board, int& temp_Solutions, int num_Solutions) {
    for (int i = 0; i < board.numRows(); ++i) {
        for (int j = 0; j < board.numColumns(); ++j) {
            if (board.getTile(i, j) != NULL) {
                std::string north = board.getTile(i, j)->getNorth();
                std::string south = board.getTile(i, j)->getSouth();
                std::string east = board.getTile(i, j)->getEast();
                std::string west = board.getTile(i, j)->getWest();
                //-------------------------------------------------------------------------
                if (i == 0 && j == 0) {
                    if(west != "pasture" || north != "pasture") return false;
                    if (board.getTile(i, j + 1) != NULL) {
                        if (east != board.getTile(i, j + 1)->getWest()) return false;
                    } else {
                        if (east == "road" || east == "city") return false;
                    }
                    if (board.getTile(i + 1, j) != NULL) {
                        if (south != board.getTile(i + 1, j)->getNorth()) return false;
                    } else {
                        if (south == "road" || south == "road") return false;
                    }
                }
                //-------------------------------------------------------------------------
                if (i == 0 && j > 0) {
                    if (north == "road" || north == "city" ) return false;
                    if (j < board.numColumns() - 1) {
                        if (board.getTile(0, j + 1) == NULL && east != "pasture") return false;
                    }
                    if (board.getTile(i, j - 1) != NULL) {
                        if (west != board.getTile(i, j - 1)->getEast()) return false;
                    } else {
                        if (west == "road" || west == "city") return false;
                    }
                    if (board.getTile(i + 1, j) != NULL) {
                        if (south != board.getTile(i + 1, j)->getNorth()) return false;
                    } else {
                        if (south == "road" || south == "city") return false;
                    }
                }
                //---------------------------------------------------------------------------
                if (j == 0 && i > 0) {
                    if (west == "road" || west == "city") return false;
                    if (i < board.numRows() - 1) {
                        if (board.getTile(i + 1, 0) == NULL && south != "pasture") return false;
                    }
                    if (board.getTile(i - 1, j) != NULL) {
                        if (north != board.getTile(i - 1, j)->getSouth()) return false;
                    } else {
                        if (north == "road" || north == "city") return false;
                    }
                    if (board.getTile(i, j + 1) != NULL) {
                        if (east != board.getTile(i, j + 1)->getWest()) return false;
                    } else {
                        if (east == "road" || east == "city") return false;
                    }
                }
                //----------------------------------------------------------------------------
                if (i > 0 && j > 0) {
                    if (j < board.numColumns() - 1) {
                        if (board.getTile(i, j + 1) == NULL && east != "pasture") return false;
                    }
                    
                    if (i < board.numRows() - 1) {
                        if (board.getTile(i + 1, j) == NULL && south != "pasture") return false;
                    }
                    
                    if (board.getTile(i - 1, j) != NULL) {
                        if (north != board.getTile(i - 1, j)->getSouth()) return false;
                    } else {
                        if (north == "road" || north == "city") return false;
                    }
                    
                    if (board.getTile(i, j - 1) != NULL) {
                        if (west != board.getTile(i, j - 1)->getEast()) return false;
                    } else {
                        if (west == "road" || west == "city") return false;
                    }
                }
                //---------------------------------------------------------------------------
                // check some special cases: （ diagonal cases )
                //----------------------------------------------------------------------------
                if (i > 0 && j < board.numColumns() - 1) {
                    if (board.getTile(i - 1, j) == NULL && board.getTile(i, j + 1) == NULL &&
                        board.getTile(i - 1, j + 1) != NULL)  return false;
                }
                
                if (i < board.numRows() - 1 && j < board.numColumns() - 1) {
                    if (board.getTile(i, j + 1 ) == NULL && board.getTile(i + 1, j) == NULL &&
                        board.getTile(i + 1, j + 1) != NULL) return false;
                }
                //----------------------------------------------------------------------------
            }
        }
    }
    ++ temp_Solutions;
    
    if (temp_Solutions > num_Solutions) {
        return true;
    } else {
        return false;
    }
}
//---------------------------------------------------------------------
// This function is used for checking the requirements of the current tile
bool Check_tile(const Board &board, Tile* tmp, int i, int j) {
    
    std::string north = tmp->getNorth();
    std::string south = tmp->getSouth();
    std::string east = tmp->getEast();
    std::string west = tmp->getWest();
    
    // First check whether the edges meet conditions:
    if (i == 0) {
        if (north == "road" || north == "city") return false;
    }
    if (j == 0) {
        if (west == "road" || west == "city") return false;
    }
    if (i == board.numRows() - 1) {
        if (south == "road" || south == "city") return false;
    }
    if (j == board.numColumns() - 1) {
        if (east == "road" || east == "city") return false;
    }
    
    // then check other tiles:
    //----------------------------------------------------------------------------
    if (j == 0 && i > 0) {
        if (board.getTile(i - 1, 0) != NULL) {
            if (north != board.getTile(i - 1, 0)->getSouth()) return false;
        }
        if (i < board.numRows() - 1) {
            if (board.getTile(i + 1, 0) != NULL) {
                if (south != board.getTile(i + 1, 0)->getNorth()) return false;
            }
        }
        if (board.getTile(i, j + 1) != NULL) {
            if (east != board.getTile(i, j + 1)->getWest()) return false;
        }
    }
    //----------------------------------------------------------------------------
    if (i == 0 && j > 0) {
        if (board.getTile(0, j - 1) != NULL) {
            if (west != board.getTile(0, j - 1)->getEast()) return false;
        }
        if (j < board.numColumns() - 1) {
            if (board.getTile(0, j + 1) != NULL) {
                if (east != board.getTile(0, j + 1)->getWest()) return false;
            }
        }
        if (board.getTile(i + 1, j) != NULL) {
            if (south != board.getTile(i + 1, j)->getNorth()) return false;
        }
    }
    //----------------------------------------------------------------------------
    if (i > 0 && j > 0) {
        if (board.getTile(i - 1, j) != NULL) {
            if (north != board.getTile(i - 1, j)->getSouth()) return false;
        }
        
        if (board.getTile(i, j - 1) != NULL) {
            if (west != board.getTile(i, j - 1)->getEast()) return false;
        }
        
        if (i < board.numRows() - 1) {
            if (board.getTile(i + 1, j) != NULL) {
                if (south != board.getTile(i + 1, j)->getNorth()) return false;
            }
        }
        
        if (j < board.numColumns() - 1) {
            if (board.getTile(i, j + 1) != NULL) {
                if (east != board.getTile(i, j + 1)->getWest()) return false;
            }
        }
    }
    //----------------------------------------------------------------------------
    return true;
}


// ==========================================================================
bool Can_place(Board &board, const std::vector<Tile*> &tiles, std::vector<Location> &locations, int index, bool allow_rotations, int& temp_Solutions, int num_Solutions) {
    
    // If all the tiles have been used up:
    if (index == tiles.size()) {
        // check if solution.
        if (Check_the_whole_board(board, temp_Solutions, num_Solutions)) {
            return true;
        } else {
            return false;
        }
    } else {
        // If not allow rotation, keep m equal to 1, which will make one following loop
        // only run one time.
        int m;
        if ( ! allow_rotations ) {
            m = 1;
        } else {
            m = 4;
        }
        
        for (int i = 0; i < board.numRows(); ++i) {
            for (int j = 0; j < board.numColumns(); ++j) {
                for (int n = 0; n < m; ++n) {
                    if (board.getTile(i, j) == NULL) {
                        Tile* tmp;
                        // Allow roatations: simply changing the tile's features
                        //-----------------------------------------------------
                        if (n == 0)  {
                            tmp = tiles[index];
                        } else if (n == 1) {
                            tmp = new Tile(tiles[index]->getWest(), tiles[index]->getNorth(), tiles[index]->getEast(),  tiles[index]->getSouth());
                        } else if (n == 2) {
                            tmp = new Tile(tiles[index]->getSouth(), tiles[index]->getWest(), tiles[index]->getNorth(), tiles[index]->getEast());
                        } else if (n == 3) {
                            tmp = new Tile(tiles[index]->getEast(), tiles[index]->getSouth(), tiles[index]->getWest(), tiles[index]->getNorth());
                        }
                        
                        // Check whether the current tile meets the requirements
                        //------------------------------------------------------
                        if (Check_tile(board, tmp, i, j)) {
                            
                            board.setTile(i, j, tmp);
                            locations.push_back(Location(i, j, 90 * n));
                            //-----------------------------------------------
                            ++ index; // Use next tile in the tiles.
                            //-----------------------------------------------
                            if (Can_place(board, tiles, locations, index, allow_rotations, temp_Solutions, num_Solutions)) {
                                return true;
                            } else {
                                board.eraseTile(i, j);
                                locations.pop_back();
                                -- index;
                            }
                        }
                    }
                }
            }
        }
        //-----------------------------------------------------
        return false;   // NO SOLUTIONS!
        //-----------------------------------------------------
    }
}




// ==========================================================================
int main(int argc, char *argv[]) {
    
    std::string filename;
    int rows = -1;
    int columns = -1;
    bool all_solutions = false;
    bool allow_rotations = false;
    HandleCommandLineArguments(argc, argv, filename, rows, columns, all_solutions, allow_rotations);
    
    // load in the tiles
    std::vector<Tile*> tiles;
    ParseInputFile(argc,argv,filename,tiles);
    
    // confirm the specified board is large enough
    if (rows < 1  ||  columns < 1  ||  rows * columns < tiles.size()) {
        std::cerr << "ERROR: specified board is not large enough" << rows << "X" << columns << "=" << rows*columns << " " << tiles.size() << std::endl;
        usage(argc,argv);
    }
    
    //----------------------------------------------------
    // If the board is very big (rows + colums > tiles.size() ),
    // we don't need to consider all the board places.
    // We can just use part of it.
    if (rows + columns > tiles.size()) {
        rows = int(tiles.size()/2);
        columns = int(tiles.size()/2);
    }

    Board board(rows,columns);
    //-----------------------------------------------------
    // Holding all the possible different solutions:
    std::vector< std::vector<Location> > Results;
    //-----------------------------------------------------
    
    std::vector<Location> locations;
    int temp_Solutions = 0;
    int num_Solutions = 0;
    int total_Solutions = 0;
    
    // If not allow all solutions or all_rotation, just find one solution:
    // Base case:
    if (!all_solutions && !allow_rotations) {
        if (Can_place(board, tiles, locations, 0, allow_rotations, temp_Solutions, num_Solutions)) {
            std:: cout << "Solution: ";
            for (int i = 0; i < locations.size(); ++i) {
                std::cout << locations[i];
            }
            std::cout << std::endl;
            board.Print();
        } else {
           std::cout << "No Solution.\n";
        }
    } else { // If allow all solutions or all_rotations
        
        bool Break_out = false;
        while (!Break_out) {
            if (Can_place(board, tiles, locations, 0, allow_rotations, temp_Solutions, num_Solutions)) {
                int count = 0;
                if (total_Solutions != 0) {
                    for (int m = 0; m < total_Solutions; m++) {
                        count = 0;
                        assert(total_Solutions == Results.size());
                        // Corner case: If the every locations can be found,
                        // We can initially conclude that this solution have been appeared in the past
                        //---------------------------------------------
                        for (int i = 0; i < tiles.size(); ++i) {
                            for (int j = 0; j < tiles.size(); ++j) {
                                if (Results[m][i] == locations[j]) {
                                    count ++;
                                }
                            }
                        }
                        if (count == tiles.size()) break;
                        //---------------------------------------------
                        // Record the " location difference "
                        int D_rows = 0;
                        int D_col = 0;
                        int D_t = 0;
                        count = 0;
                        for (int n = 0; n < tiles.size(); n++) {
                            if (n == 0) {
                                D_col = locations[n].column - Results[m][n].column;
                                D_rows = locations[n].row - Results[m][n].row;
                                D_t = locations[n].rotation - Results[m][n].rotation;
                            }
                            
                            if ((D_col == locations[n].column - Results[m][n].column) &&
                                (D_rows == locations[n].row - Results[m][n].row) &&
                                (D_t == locations[n].rotation - Results[m][n].rotation)) {
                                count++;
                            } else {
                                for (int i = 0; i < tiles.size(); i++) {
                                    
                                    if ((n != i) && (tiles[i]->getNorth() == tiles[n]->getNorth()) &&
                                        (tiles[i]->getSouth() == tiles[n]->getSouth()) &&
                                        (tiles[i]->getEast() == tiles[n]->getEast()) &&
                                        (tiles[i]->getWest() == tiles[n]->getWest())) {
                                        
                                        if ((D_col == locations[i].column - Results[m][n].column) &&
                                            (D_rows == locations[i].row - Results[m][n].row) &&
                                            (D_t == locations[i].rotation - Results[m][n].rotation)) {
                                            count++;
                                        }
                                    }
                                }
                            }
                        }
                        // If all the tiles are the same, break out this loop,
                        // since we find the same solution as before and we don't
                        // need to find another one:
                        if (count == tiles.size()) break;
                    }
                }

                if (count != tiles.size()) {
                    std:: cout << "Solution: ";
                    for (int i = 0; i < locations.size(); ++i) {
                        std::cout <<locations[i];
                    }
                    std::cout << std::endl;
                    board.Print();
                    total_Solutions ++;
                    //--------------------------------
                    Results.push_back(locations);
                    //--------------------------------
                }
                //--------------------------------
                // If there are no different solution,
                // we must set all the variables to the orignal status:
                board.clear();
                locations.clear();
                temp_Solutions = 0;
                ++ num_Solutions;
                
            } else {
                Break_out = true;
            }
        } 
        if (num_Solutions == 0)  std::cout << "No Solution.\n";
        else std::cout << "Found " << total_Solutions << " Solution(s).\n" ;
    }
    
    // delete the tiles
    for (int t = 0; t < tiles.size(); t++) {
        delete tiles[t];
    }
    return 0;
}
// ===================================================================================













