#include <Graphics/TileMap.hpp>
#include <glm/gtx/transform.hpp>

using namespace Graphics;

TileMap::TileMap( std::shared_ptr<SpriteSheet> spriteSheet, uint32_t columns, uint32_t rows )
: columns { columns }
, rows { rows }
, spriteSheet { std::move( spriteSheet ) }
, spriteGrid( static_cast<size_t>( columns ) * rows, -1 )
{}

int TileMap::operator()( size_t i, size_t j ) const noexcept
{
    if ( i < rows && j < columns )
        return spriteGrid[i * columns + j];

    return -1;
}

int& TileMap::operator()( size_t i, size_t j ) noexcept
{
    assert( i < rows );
    assert( j < columns );

    return spriteGrid[i * columns + j];
}

void TileMap::clear()
{
    std::ranges::fill( spriteGrid, -1 );
}

void TileMap::setSpriteGrid( std::span<const int> _spriteGrid )
{
    spriteGrid = std::vector( _spriteGrid.begin(), _spriteGrid.end() );
}

void TileMap::draw( Image& image, const glm::mat3& transform ) const
{
    if ( !spriteSheet )
        return;

    const int spriteWidth  = static_cast<int>( spriteSheet->getSpriteWidth() );
    const int spriteHeight = static_cast<int>( spriteSheet->getSpriteHeight() );
    const int numSprites   = static_cast<int>( spriteSheet->getNumSprites() );

    int x = 0;
    int y = 0;
    for ( uint32_t i = 0u; i < rows; ++i )
    {
        x = 0;
        for ( uint32_t j = 0; j < columns; ++j )
        {
            const int spriteId = spriteGrid[i * columns + j];
            if ( spriteId >= 0 && spriteId < numSprites )
            {
                glm::mat3 t = {
                    1, 0, 0,
                    0, 1, 0,
                    x, y, 1
                };
                image.drawSprite( spriteSheet->getSprite( spriteId ), transform * t);
            }
            x += spriteWidth;
        }
        y += spriteHeight;
    }
}
