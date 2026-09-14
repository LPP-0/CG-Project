import math, random

def generate_belt(num_asteroids):
    output = "<group>\n"
    
    for _ in range(num_asteroids):

        radius = random.uniform(3.2, 4.7) 
        angle = random.uniform(0, 2 * math.pi)
        
        x = radius * math.cos(angle)
        z = radius * math.sin(angle)
        y = random.uniform(-0.15, 0.15)
        
        shape = random.choice(['sphere.3d', 'box.3d', 'cone.3d'])
        
        if shape == 'sphere.3d':
            scale = random.uniform(0.01, 0.025)
            output += f'\t<group><transform><translate x="{x:.3f}" y="{y:.3f}" z="{z:.3f}"/><scale x="{scale:.3f}" y="{scale:.3f}" z="{scale:.3f}"/></transform><models><model file="sphere.3d"/></models></group>\n'
        else:
            scale = random.uniform(0.01, 0.025)
            rot = random.randint(0, 360)
            output += f'\t<group><transform><translate x="{x:.3f}" y="{y:.3f}" z="{z:.3f}"/><rotate angle="{rot}" x="1" y="1" z="0"/><scale x="{scale:.3f}" y="{scale:.3f}" z="{scale:.3f}"/></transform><models><model file="{shape}"/></models></group>\n'
            
    output += "</group>"
    with open("belt.xml", "w") as f:
        f.write(output)


generate_belt(360)
print("Novo belt.xml gerado!")