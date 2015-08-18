require 'lua/table'

require 'lua/matrix'

require 'lua/vec3'
require 'lua/vec4'

require 'lua/mat3'
require 'lua/mat4'

--- Linear interpolation
function mix(x, y, alpha)
    return x + (y-x) * alpha
end