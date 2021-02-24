#shader vertex
#version 330 core 
    
//layout(qualifier1​, qualifier2​ = value, ...) variable definition  
//Vertex shader inputs can specify the attribute index that the particular input uses.  
layout(location = 0) in vec4 position;  

void main()  
{  
    gl_Position = position;  
};


#shader fragment
#version 330 core  
    
layout(location = 0) out vec4 color; 

void main()  
{  
    color = vec4(1.0, 0.2, 0.3, 1.0);  
};