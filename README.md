# SoftRendererGL

基于OpenGL的Shader练习工程，基于教程：https://learnopengl-cn.github.io/

backup文件夹仅开发过程中废弃代码备份用，无实际功能意义



# 学习笔记

## PBR部分
### IBL预计算贴图
IBL是用预烘焙的环境贴图计算辐射，因此PBR方程中的积分项不再像N个点光源一样可以逐个计算并累加，而是需要通过黎曼和近似积分，性能消耗实时渲染无法负担  
因此需要将方程中的每一项都预先烘焙好对应的贴图，渲染时直接对贴图采样即可。IBL贴图共三部分：  
1. 漫反射辐照贴图(CubeMap)
计算每个法向N所在半球的积分, 使用球坐标系的单位面积黎曼和近似模拟积分

2. 预过滤环境贴图(CubeMap With Mipmap)
以预卷积时的采样方向作为反射方向(N = V = R)，通过低差异序列以及GGX重要性采样根据表面粗糙度生成随机的表面半向量(H)  
根据N和H推出反射向量L, 作为采样的方向向量  
粗糙度越高, 随机生成的H差异越大, 对应粗糙度下的mipmap越模糊, 用于对应不同粗糙程度表面的反射效果  

3. BRDF积分预计算LUT(Texture_2D)  
BRDF方程约分F项后, 取横坐标NdotV, 纵坐标roughness绘制所得的2D纹理, 与具体贴图环境无关


# TODO List
1. TextureLoader与TextureAllocator融合  
2. RenderUtil去除, 对应功能合并至TextureAllocator
3. 多种Render合并, 简化类数量并梳理继承关系