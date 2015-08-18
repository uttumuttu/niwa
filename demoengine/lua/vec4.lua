vec4 = {}
vec4_mt = { __index = vec4 }

function vec4:new(args)
    return setmetatable(args, vec4_mt)
end

vec4_mt.__add = function (a,b)
    return vec4:new{a[1]+b[1], a[2]+b[2], a[3]+b[3], a[4]+b[4]}
end

vec4_mt.__sub = function (a,b)
    return vec4:new{a[1]-b[1], a[2]-b[2], a[3]-b[3], a[4]-b[4]}
end

vec4_mt.__mul = function (a,b)
    return vec4:new{a[1]*b, a[2]*b, a[3]*b, a[4]*b}
end

function vec4:length()
    return math.sqrt(
        self[1]*self[1] + self[2]*self[2] + 
        self[3]*self[3] + self[4]*self[4])
end

function vec4:normalized()
    return self * (1 / self:length());
end