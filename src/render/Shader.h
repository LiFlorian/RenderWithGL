﻿#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    // 程序ID, 即编译链接而成的ShaderProgram
    unsigned int ID;

public:
    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

    ~Shader();

    // 使用/激活程序
    void Use();

    // uniform工具函数
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
	void SetVec3(const std::string& name, glm::vec3 value) const;
	void SetMat4(const std::string& name, glm::mat4 value) const;
	void SetMat3(const std::string& name, glm::mat3 value) const;

	void SetParaLightParams();
	void SetPointLightParams(glm::vec3 LightPos);
	void SetSpotLightParams();
};

#endif
