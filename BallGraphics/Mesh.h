#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "ComPtr.h"
#include "D3D.h"
#include <functional>
#include "Vector.h"
#include "Camera.h"

namespace BallGraphics
{

class Mesh
{
public:

    Mesh( const D3D& d3d ) noexcept;
    Mesh( const Mesh& arg ) = delete;
    Mesh& operator = ( const Mesh& arg ) = delete;
    Mesh( Mesh&& arg ) noexcept;
    Mesh& operator = ( Mesh&& arg ) noexcept;

    virtual ~Mesh()
    {
    };

    using GetWorldFunc = std::function<DirectX::XMMATRIX()>;
    virtual void render( const GetWorldFunc& worldFunc, const Camera* camera ) noexcept = 0;

protected:
    const D3D& d3d_;

    BallUtils::ComPtr<ID3D11Buffer> vertexBuffer_{ nullptr };
    BallUtils::ComPtr<ID3D11Buffer> indexBuffer_{ nullptr };
};

} //namespace