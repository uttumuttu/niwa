require 'lua/math'

local sin = math.sin -- shorthands for common mathematical functions
local cos = math.cos
local pow = math.pow

load_dll("fx_raytrace.dll")
load_dll("fx_levelset.dll")
load_dll("fx_dynamics.dll")
load_dll("fx_graphics.dll")

local clear = clear{color={0,0,0,1}}

local mandelbrot = full_screen_shader{
    vertex_shader = "shaders/mandelbrot.vert",
    fragment_shader = "shaders/mandelbrot.frag",
    
    scale = function (t)
        local s = (1 + cos(t/2)) / 2 
        return pow(s, 3) + 2e-5
    end
}

local julia = full_screen_shader{
    vertex_shader = "shaders/julia.vert",
    fragment_shader = "shaders/julia.frag",
    
    camera_rotation = function (t)
        return rotation_matrix(t*10, 1, 0, 0) *
               rotation_matrix(t*15, 0, 0, 1)
    end,

    fractal_constant = function (t)
        local q1 = vec4:new{-0.3, 0.8, 0, 0}
        local q2 = vec4:new{ 0, -0.8, 0.3, 0.1}

        local alpha = (1 + cos(t)) * .5

        return mix(q1,q2,alpha):normalized() 
            * mix(q1:length(), q2:length(), alpha)
    end,
    
    diffuse_color = function (t)
        local c1 = vec4:new{0.75, 0.25, 1.0, 0}
        local c2 = vec4:new{1.0, 0.8, 0.2, 0}

        local alpha = (1 + cos(t)) * .5
        
        return mix(c1, c2, alpha)
    end
}

local postprocess = postprocess_demo()

local raytrace = raytrace_effect{
    window_size = {160*2, 120*2},
    photon_count = 25000 * 1 * 0,
    photon_query_type = "range_query",
    --photon_query_type = "neighbor_query",
    photon_query_radius = 0.3,
    photon_query_neighbor_count = 30,
    objects={
        --[[mesh{
            model="data/models/knot.3ds",
            bounds={{-0.9,-1,-0.9}, {0.9,0.25,0.9}}
        },--]]
        sphere{
            position=[(t) | {-0.5, 0.4 * sin(t), 0.5}],
            radius=0.4, material="specular", color={1.0, 1.0, 1.0}
        },
        sphere{
            position=[(t) | {-0.5, -0.4 * sin(t), -0.5}],
            radius=0.4,    material="diffuse", color={0.5, 0.5, 0.5}
        },
        sphere{
            position=[(t) | {0.5, 0.4 * cos(t), 0.5}],
            radius=0.4,    material="dielectric",
            refractive_index=[(t) | sin(t) + 2]
        },
        cornell_box_walls()
    },
    lights={
        --[[square_light{
            power={20,20,20}, position={0,0.99,0},
            basis_1={0.4, 0, 0}, basis_2={0, 0, 0.4},
            sample_count = 4
        },--]]
        square_light{
            power={20, 20, 20}, position={-.5, 0.99, 0}, 
            basis_1={0.4, 0, 0}, basis_2={0, 0, 0.8},
            sample_count = 4
        },
        square_light{
            power={20, 20, 20}, position={.5, 0.99, 0}, 
            basis_1={0.4, 0, 0}, basis_2={0, 0, 0.8},
            sample_count = 4
        }
    },
    camera = camera{
        position = [(t) | {cos(t*3/5), sin(t*3/5), -3}],
        --position = {0,0,-3},
        lookat = {0,0,0}
    }
}

--local caustics = hardware_caustics{
    --floor_texture = "data/textures/wood.jpg"}

--add_span{clear, 0, math.huge, layer=0}
--add_span{julia, 0, math.huge, layer=1}

--add_span{mandelbrot, 0, math.huge}

--add_span{cloth(), 0, math.huge}

add_span{raytrace, 0, math.huge}

--add_span{postprocess, 0, math.huge}

--add_span{caustics, 0, math.huge}

--add_span{quaternion_julia(), 0, math.huge}

--add_span{morphing{
    --source_model="data/models/skull.3ds",
    --destination_model="data/models/dragon.3ds",
    --resolution=100,
    --vertex_normals=1}, 0, math.huge}
    
--add_span{rigid_body_effect(), 0, math.huge}