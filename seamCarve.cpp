/*  Algorithms Fall 2019 : Project 4 - Seam Carving
    Brighid Harris - bah128 - 3154693
    12.3.2019

    This program will take the requested pgm file and remove 
    the specified number of vertical and horizontal seams

    ./p4 [ filename ] [ vert ] [ hori ]:
        Will remove [ vert ] vertical seams and [ hori ] horizontal
        seams from the image and generate [ filename ]_processed.pgm
        once complete 
*/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Ties all necessary values to a single struct
struct triple{
    short pixel;
    short energy;
    int cumlEng;
    triple() : pixel( 0 ), energy( 0 ), cumlEng( 0 ) {}
};

using grayLine = std::vector< triple >;
using grayMap = std::vector< grayLine >;

bool readFile( std::string & filename, grayMap & img );
void removeSeams( std::string filename, grayMap & img, int vert, int hor );

int main(int argc, char *argv[])
{
    if( argc != 4  )
    {
        std::cout << "Not enough arguments, command must be in form: \n   ./[prog] [filaname] [verticle] [horizontal]\n";
        return 0;
    }

    std::string filename = argv[1];
    int vertical = std::stoi( argv[2] ), horizontal = std::stoi( argv[3] );
    grayMap img;

    if( readFile( filename, img ) )
    {
        if( img.size() <= horizontal || img[0].size() <= vertical )
        {
            std::cout << "Cannot remove that many seams\n";
            return 0;
        }
        removeSeams( filename, img, vertical, horizontal );
    }
    return 0;
}

// Reads the file and initializes the matrix
bool readFile( std::string & filename, grayMap & img )
{
    // Opens file if it exists in the current folder
    std::ifstream file( filename.c_str(), std::ios::binary );
    if( !file )
    {
        std::cout << filename << " could not be opened.\n";
        return 0;
    }
    int height, width;
    std::string trash;
    // Gets title / P2
    std::getline( file, trash );
    // Gets comment or width
    file >> trash;
    if( trash[0] == '#' )
    {
        std::getline( file, trash );
        file >> width >> height;
    }
    else
    {
        width = std::stoi( trash );
        file >> height;
    }
    file >> trash;
    for(; height > 0; --height ) 
    {
        grayLine line;
        line.resize( width );
        for( auto j = line.begin(); j != line.end(); ++j )
            file >> j->pixel;
        img.push_back( line );
    }
    file.close();
    return 1;
}

// Calculates the energy of the cell located at pix
void calcEnergy( grayMap & img, grayMap::iterator row, grayLine::iterator pix )
{
    pix->energy = 0;
    // Left Cell
    if( pix != row->begin() )
        pix->energy += std::abs( pix->pixel - ( pix - 1 )->pixel );
    // Right Cell
    if( pix != row->end() - 1 )
        pix->energy += std::abs( pix->pixel - ( pix + 1 )->pixel );
    // Upper Cell
    int index = pix - row->begin();
    if( row != img.begin() )
        pix->energy += std::abs( pix->pixel - ( ( row - 1 )->begin() + index )->pixel );
    // Lower Cell
    if( row != img.end() - 1 )
        pix->energy += std::abs( pix->pixel - ( ( row + 1 )->begin() + index )->pixel );
}

// Sets the inital energy of all cells
void getEnergy( grayMap & img )
{
    for( auto i = img.begin(); i != img.end(); ++i )
    {
        for( auto j = i->begin(); j != i->end(); ++j )
            calcEnergy( img, i, j );
    }
}

// Calculates the culmative energy of each cell in the matrix
void getCE( grayMap & img )
{
    int width = img[0].size();

    // Set initial row
    for( auto i = img[0].begin(); i != img[0].end(); ++i )
        i->cumlEng = i->energy;
    
    for( auto prev = img.begin(), row = img.begin() + 1; row != img.end(); ++prev, ++row )
    {
        auto small = prev->begin(), test = prev->begin();
        for( auto i = row->begin(); i != row->end(); ++i, small = ++test )
        {
            // Catches if the image is only 1 pixel wide
            if( width > 1 )
            {
                // Left edge
                if( small == prev->begin() )
                {
                    if( ( small + 1 )->cumlEng < small->cumlEng )
                        ++small;
                }
                // Right edge
                else if( small == prev->end() - 1 )
                {
                    if( ( small - 1 )->cumlEng <= small->cumlEng )
                        --small;
                }
                // Middle
                else
                {
                    if( ( test - 1 )->cumlEng <= small->cumlEng )
                        --small;
                    if( ( test + 1 )->cumlEng < small->cumlEng )
                        small = test + 1;
                }
            }
            i->cumlEng = i->energy + small->cumlEng;
        }
    }
}

// Removes num verticle seams from the image
void rm( grayMap & img, int num )
{
    for(; num > 0; --num )
    {
        // Calculates cumlative energy matrix
        getCE( img );

        // Find smallest value
        auto row = ( img.end() - 1 );
        auto small = row->begin();
        for( auto tst = (row->begin() + 1 ); tst != row->end(); ++tst )
        {
            if( tst->cumlEng < small->cumlEng )
                small = tst;
        }
        
        // Erases the smallest value and recalculates the energy of surrounding cells
        row->erase( small );
        // left cell
        if( small != row->begin() )
            calcEnergy( img, row, small - 1 );
        // All right cells
        for( auto calc = small; calc != row->end(); ++calc )
            calcEnergy( img, row, calc );

        for( --row; row != img.begin() - 1; --row )
        {
            // Reposition small and recalculate the energy of that cell
            small = row->begin() + ( small - ( row + 1 )->begin() );

            // Find smallest cuml value
            // Left edge
            if( small == row->begin() )
            {
                if( ( small + 1 )->cumlEng < small->cumlEng )
                    ++small;
            }
            // Right edge
            else if( small == row->end() - 1 )
            {
                if( ( small - 1 )->cumlEng <= small->cumlEng )
                    --small;
            }
            // Middle
            else
            {
                auto tst = small;
                // Left
                if( ( tst - 1 )->cumlEng <= small->cumlEng )
                    --small;
                // Right
                if( ( ++tst )->cumlEng < small->cumlEng )
                    small = tst;
            }
            row->erase( small );

            // Recalculates the energy of the left cell and all cells to the right
            // of the erased value
            // Left cell
            if( small != row->begin() )
                calcEnergy( img, row, small - 1 );
            // All right cell
            for( auto calc = small; calc != row->end(); ++calc )
                calcEnergy( img, row, calc );
            if( row != img.end() - 1 )
            {
                int index = small - row->begin();
                ++row;
                // Recalculate all lower cell to the right
                for( auto calc = row->begin() + index; calc != row->end(); ++calc )
                    calcEnergy( img, row, calc );
                --row;
            }
        }
    }
}

/* Rotates the image so that the result looks like:
                   AFK
    ABCDE          BGL
    FGHIJ   --->   CHM      Transposes Matrix
    KLMNO          DIN
                   EJO
*/
void rotate( grayMap & img )
{
    int height = img.size(), width = img[0].size();
    for( int i = 0; i < width; ++i )
    {
        grayLine line;
        line.resize( height );
        // Grabs the leftmost verticle line and create a vector based on
        // those values
        for( int j = 0; j < height; ++j )
            line[j] = img[ j ][ i ];
        img.push_back( line );
    }
    // Erases the previous, unrotated image
    img.erase( img.begin(), img.begin() + height);
}

// Creates the processed pgm file
void write( std::string filename, grayMap & img )
{
    // Returns image to it's original orientation
    rotate( img );
    
    // Get processed filename and open file
    filename = filename.substr( 0, filename.find_last_of('.') );
    filename += "_processed.pgm";
    std::ofstream outFile( filename.c_str(), std::ios::binary );

    // Writes inital Lines necessary for the gray map
    outFile << "P2\n" << img[0].size() << ' ' << img.size() << "\n255\n";

    // Writes all the gray values to the file
    for( auto i = img.begin(); i != img.end(); ++i )
    {
        for( auto j = i->begin(); j != i->end(); ++j )
            outFile << j->pixel << ' ';
        outFile << '\n';
    }
    outFile.close();
}

// Driver function for all the other functions
void removeSeams( std::string filename, grayMap & img, int vert, int hor )
{
    getEnergy( img );
    if( vert > 0 ) 
    {
        std::cout << "Removing " << vert << " vertical seams...\n";
        rm( img, vert );
    }
    rotate( img );
    if( hor > 0 )
    {
        std::cout << "Removing " << hor << " horizontal seams...\n";
        rm( img, hor );
    }
    write( filename, img );
    std::cout << "Done\n";
}