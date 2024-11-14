#include "TestTexture2D.h"

#include "Core.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp" // glm::ortho
#include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

namespace test {

    TestTexture2D::TestTexture2D()
        : m_ProjectionMatrix(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), 
        m_ViewMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
        m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
    {
        /* Define 3 vertices as an array of floats that will make up a triangle once linked together */
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
             50.0f, -50.0f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 1.0f, 1.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        /* By default, OpenGL does not consider source alpha so enable and set blend mode to consider it */
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        /* Setup re-usable assets */
        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Texture = std::make_unique<Texture>("res/textures/GamesXboxHubAppList.scale-200_contrast-high.png");

        /* Tell OpenGL how the vertices are going to be laid out */
        m_VertexArray = std::make_unique<VertexArray>();

        /* Tell OpenGL that we need to store these positions as an array and select them to be drawn on screen */
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VertexArray->AddBuffer(*m_VertexBuffer, layout);

        /* Tell OpenGL that we want to re-use "redundant" vertex information by telling it the indices of the vertex we want to draw */
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader->Bind();
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestTexture2D::~TestTexture2D()
    {
    }

    void TestTexture2D::OnUpdate(float deltaTime)
    {

    }

    void TestTexture2D::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        m_Texture->Bind();

        {
            glm::mat4 modelMatrixA = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvpA = m_ProjectionMatrix * m_ViewMatrix * modelMatrixA;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvpA);

            renderer.Draw(*m_VertexArray, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 modelMatrixB = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvpB = m_ProjectionMatrix * m_ViewMatrix * modelMatrixB;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvpB);

            renderer.Draw(*m_VertexArray, *m_IndexBuffer, *m_Shader);
        }
    }

    void TestTexture2D::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

}