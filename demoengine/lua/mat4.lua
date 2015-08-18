mat4 = {}
mat4_mt = { __index = mat4 }

function mat4:new(args)
    return setmetatable(args, mat4_mt)
end

function mat4:eye()
    return mat4:new{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}
end

mat4_mt.__mul = function (lhs, rhs)
    return mat4:new(mat_mul(4,4,4,lhs,rhs))
end

--- Creates a rotation matrix similar to OpenGL's glRotate
function rotation_matrix(alpha, x, y, z)
    local l = math.sqrt(x*x + y*y + z*z)
    
    x = x / l
    y = y / l
    z = z / l
    
    local S = mat3:new{{0,-z,y}, {z,0,-x}, {-y,x,0}}

    local u = vec3:new{x,y,z}
    
    local rad = math.rad(alpha)
    
    local M = u:outer(u) + (mat3:eye() - u:outer(u)) * math.cos(rad) + S * math.sin(rad)
    
    local R = mat4:eye()
    
    for i=1,3 do
        for j=1,3 do
            R[i][j] = M[i][j]
        end
    end
    
    return R
end