/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MULTI__
#define __MULTI__

#include <tucano.hpp>
#include <camera.hpp>
#include <photo/multiTextureManagerObj.h>
#include <depthMap.hpp>
#include <framebuffer.hpp>

using namespace Tucano;

namespace Effects
{

/**
 * @brief Renders a mesh using a Phong shader.
 */
class Multi : public Effect
{

private:

    /// Phong Shader
    Shader phong_shader;
    Shader depthMap;
    Shader multiTF;
    Shader mPassRenderLoop;
    Shader mPassRender;

    Shader simplePass;
    Shader showFBO;

    /// Default color
    Eigen::Vector4f default_color;

    Framebuffer *fbo;
    Framebuffer *fboMPass;

    Framebuffer *fbo_01;
    Framebuffer *fbo_02;

    Framebuffer *readFbo;
    Framebuffer *writeFbo;

    Mesh quad;

    GLuint writeBuffer;
    GLuint readBuffer;

    int ID_DepthTexture;
    bool firstRenderFlag;

public:

    /**
     * @brief Default constructor.
     */
    Multi (void)
    {
        default_color << 0.7, 0.7, 0.7, 1.0;
        fbo =0;
        fboMPass = 0;
        fbo_01 = 0;
        fbo_02 = 0;

        ID_DepthTexture = 0;

        writeBuffer = 0;
        readBuffer = 1;
    }

    /**
     * @brief Default destructor
     */
    virtual ~Multi (void) {
        delete fbo;
        delete fboMPass;
        delete fbo_01;
        delete fbo_02;
    }

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(depthMap,       "depthmap");
        loadShader(phong_shader,   "phongshader") ;
        loadShader(mPassRenderLoop,"multipassLoop");
        loadShader(mPassRender,    "multipass");

        loadShader(simplePass,     "simplepass");
        loadShader(showFBO,        "showFbo");

        //Initialize multiTF using coordtf shaders
        multiTF.load("coordtf", shaders_dir);
        const char * vars[] = {"nCoord"};
        multiTF.initializeTF(1,vars);
        shaders_list.push_back(&multiTF);

        fbo      = new Framebuffer();
        fboMPass = new Framebuffer();
        fbo_01   = new Framebuffer();
        fbo_02   = new Framebuffer();

        quad.createQuad();

        firstRenderFlag = true;
    }

	/**
	* @brief Sets the default color, usually used for meshes without color attribute
	*/
	void setDefaultColor ( Eigen::Vector4f& color )
	{
		default_color = color;
	}

    /** * @brief Render the mesh given a camera and light, using a Phong shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void depthMapRender (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fbo->getWidth() != viewport_size[0] || fbo->getHeight() != viewport_size[1])
        {
            fbo->create(viewport_size[0], viewport_size[1], 1);
        }

        fbo->clearAttachments();
        fbo->bindRenderBuffer(ID_DepthTexture);

            depthMap.bind();

            depthMap.setUniform("projectionMatrix", camera.getProjectionMatrix());
            depthMap.setUniform("modelMatrix", mesh.getModelMatrix());
            depthMap.setUniform("viewMatrix", camera.getViewMatrix());
            depthMap.setUniform("near", 0.1);
            depthMap.setUniform("far", 10000.0);

            mesh.setAttributeLocation(depthMap);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            depthMap.unbind();

        fbo->unbind();
        fbo->clearDepth();
    }

    void updateTF (MultiTextureManagerObj& multiTexObj, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Mesh &mesh = *multiTexObj.getMesh();
        Eigen::Vector2f viewport = Eigen::Vector2f(camera.getViewport()[2], camera.getViewport()[3]);
        vector <Eigen::Vector2f > blankbuffer (mesh.getNumberOfVertices(), Eigen::Vector2f::Zero());

        string id = "imageID_";
        id += std::to_string(multiTexObj.getCurrentPhotoIndex());

        multiTF.bind();
            glEnable(GL_RASTERIZER_DISCARD);
                multiTF.setUniform("in_Viewport", viewport);
                multiTF.setUniform("projectionMatrix", camera.getProjectionMatrix());
                multiTF.setUniform("modelMatrix", mesh.getModelMatrix());
                multiTF.setUniform("viewMatrix", camera.getViewMatrix());
                multiTF.setUniform("depthMapTexture", fbo->bindAttachment(ID_DepthTexture));
                if(!mesh.hasAttribute(id))
                {
                    mesh.createAttribute(id, blankbuffer);
                }
                mesh.bindBuffers();
                    mesh.getAttribute("in_Position")->enable(multiTF.getAttributeLocation("in_Position"));
                    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mesh.getAttribute(id)->getBufferID());
                        glBeginTransformFeedback(GL_POINTS);
                            glEnable(GL_DEPTH_TEST);
                            mesh.renderPoints();
                        glEndTransformFeedback();
                    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
                mesh.unbindBuffers();
            glDisable(GL_RASTERIZER_DISCARD);
        multiTF.unbind();
        fbo->unbindAttachments();
    }

    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        phong_shader.bind();

        // sets all uniform variables for the phong shader
        phong_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        phong_shader.setUniform("modelMatrix", mesh.getModelMatrix());
        phong_shader.setUniform("viewMatrix", camera.getViewMatrix());
        phong_shader.setUniform("lightViewMatrix", lightTrackball.getViewMatrix());
        phong_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
        phong_shader.setUniform("default_color", default_color);

        mesh.setAttributeLocation(phong_shader);

        glEnable(GL_DEPTH_TEST);
        mesh.render();

        phong_shader.unbind();
    }

    void renderFbo(Tucano::Framebuffer& renderFBO ,Tucano::Mesh& renderMesh, GLuint renderBuffer = 0)
    {
        showFBO.bind();

        Eigen::Vector2f viewport = Eigen::Vector2f(renderFBO.getWidth(), renderFBO.getHeight());

        showFBO.setUniform("in_Viewport", viewport);
        showFBO.setUniform("imageTexture", renderFBO.bindAttachment(renderBuffer));

        renderMesh.render();

        showFBO.unbind();
        renderFBO.unbindAttachments();
    }

    void render(MultiTextureManagerObj &multiTextObj, const Camera &camera, const Camera &lightTrackball)
    {
        Mesh &mesh = *multiTextObj.getMesh();
        if(firstRenderFlag){
            Tucano::Camera cam;
            cam.setViewport(camera.getViewport());
            for(int i = 0; i < multiTextObj.getNumPhotos(); i++){
                multiTextObj.calibrateCamera(cam);
                depthMapRender(*multiTextObj.getMesh(), cam, lightTrackball);
                updateTF(multiTextObj, cam, lightTrackball);
                multiTextObj.nextPhoto();
            }
            multiTextObj.changePhotoReferenceTo(0);
            firstRenderFlag = false;
         }
//        render(mesh, camera, lightTrackball);
        renderMultiPass(multiTextObj, camera, lightTrackball);

//        renderMultiPassLoop(multiTextObj, camera, lightTrackball);
//        renderSimplePass(multiTextObj, camera, lightTrackball);

    }
    void renderSimplePass(MultiTextureManagerObj& multiTexObj, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Mesh &mesh = *multiTexObj.getMesh();
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewPort_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewPort_size[0] * size, viewPort_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMPass->getWidth() != viewPort_size[0] || fboMPass->getHeight() != viewPort_size[1])
        {
            fboMPass->create(viewPort_size[0], viewPort_size[1], 2);
        }

        fboMPass->clearAttachments();

        fboMPass->clearAttachment(writeBuffer);
        fboMPass->bindRenderBuffer(writeBuffer);
            simplePass.bind();
                simplePass.setUniform("projectionMatrix",  camera.getProjectionMatrix());
                simplePass.setUniform("modelMatrix",       mesh.getModelMatrix());
                simplePass.setUniform("viewMatrix",        camera.getViewMatrix());
                simplePass.setUniform("lightViewMatrix",   lightTrackball.getViewMatrix());

//                simplePass.setUniform("viewportSize", Eigen::Vector2f(viewPort_size[0], viewPort_size[1]));

                simplePass.setUniform("lastPassTexture",   fboMPass->bindAttachment(readBuffer));

                simplePass.setUniform("imageTexture_0", multiTexObj.getBaseTextureAt(0)->bind());
                simplePass.setUniform("imageTexture_1", multiTexObj.getBaseTextureAt(1)->bind());
                simplePass.setUniform("imageTexture_2", multiTexObj.getBaseTextureAt(2)->bind());
                simplePass.setUniform("imageTexture_3", multiTexObj.getBaseTextureAt(3)->bind());
                simplePass.setUniform("imageTexture_4", multiTexObj.getBaseTextureAt(4)->bind());
                simplePass.setUniform("imageTexture_5", multiTexObj.getBaseTextureAt(5)->bind());
                simplePass.setUniform("imageTexture_6", multiTexObj.getBaseTextureAt(6)->bind());
                simplePass.setUniform("imageTexture_7", multiTexObj.getBaseTextureAt(7)->bind());
                simplePass.setUniform("imageTexture_8", multiTexObj.getBaseTextureAt(8)->bind());
                simplePass.setUniform("imageTexture_9", multiTexObj.getBaseTextureAt(9)->bind());

                glEnable(GL_DEPTH_TEST);

                mesh.bindBuffers();

                mesh.getAttribute("imageID_0")->enable(simplePass.getAttributeLocation("in_coordText_0"));
                mesh.getAttribute("imageID_1")->enable(simplePass.getAttributeLocation("in_coordText_1"));
                mesh.getAttribute("imageID_2")->enable(simplePass.getAttributeLocation("in_coordText_2"));
                mesh.getAttribute("imageID_3")->enable(simplePass.getAttributeLocation("in_coordText_3"));
                mesh.getAttribute("imageID_4")->enable(simplePass.getAttributeLocation("in_coordText_4"));
                mesh.getAttribute("imageID_5")->enable(simplePass.getAttributeLocation("in_coordText_5"));
                mesh.getAttribute("imageID_6")->enable(simplePass.getAttributeLocation("in_coordText_6"));
                mesh.getAttribute("imageID_7")->enable(simplePass.getAttributeLocation("in_coordText_7"));
                mesh.getAttribute("imageID_8")->enable(simplePass.getAttributeLocation("in_coordText_8"));
                mesh.getAttribute("imageID_9")->enable(simplePass.getAttributeLocation("in_coordText_9"));


//                mesh.getAttribute("in_Position")->enable(simplePass.getAttributeLocation("in_Position"));
//                mesh.getAttribute("in_Normal")->enable(mPassRender.getAttributeLocation("in_Normal"));

                mesh.renderElements();
                mesh.unbindBuffers();

                for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                {
                    multiTexObj.getBaseTextureAt(i)->unbind();
                }

            simplePass.unbind();

        fboMPass->unbind();

        //SWAP
        GLuint temp = readBuffer;
        readBuffer = writeBuffer;
        writeBuffer = temp;

        renderFbo(*fboMPass, quad, readBuffer);

    }

    void renderMultiPassOld(MultiTextureManagerObj& multiTexObj, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Mesh &mesh = *multiTexObj.getMesh();
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewPort_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewPort_size[0] * size, viewPort_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMPass->getWidth() != viewPort_size[0] || fboMPass->getHeight() != viewPort_size[1])
        {
            fboMPass->create(viewPort_size[0], viewPort_size[1], 2);
        }

        fboMPass->clearAttachments();

        fboMPass->clearAttachment(writeBuffer);
        fboMPass->bindRenderBuffer(writeBuffer);
            mPassRenderLoop.bind();
                mPassRenderLoop.setUniform("projectionMatrix",  camera.getProjectionMatrix());
                mPassRenderLoop.setUniform("modelMatrix",       mesh.getModelMatrix());
                mPassRenderLoop.setUniform("viewMatrix",        camera.getViewMatrix());
                mPassRenderLoop.setUniform("lightViewMatrix",   lightTrackball.getViewMatrix());
                mPassRenderLoop.setUniform("firstPass",         true);
                mPassRenderLoop.setUniform("lastPass",          false);
                mPassRenderLoop.setUniform("multiPass",         false);
                mPassRenderLoop.setUniform("viewportSize", Eigen::Vector2f(viewPort_size[0], viewPort_size[1]));

                mPassRenderLoop.setUniform("lastPassTexture",   fboMPass->bindAttachment(readBuffer));

                mPassRenderLoop.setUniform("imageTexture_0", multiTexObj.getBaseTextureAt(0)->bind());


                glEnable(GL_DEPTH_TEST);
                mesh.bindBuffers();
                mesh.getAttribute("imageID_0")->enable(mPassRenderLoop.getAttributeLocation("in_coordText_0"));
                mesh.getAttribute("in_Position")->enable(mPassRenderLoop.getAttributeLocation("in_Position"));
                mesh.getAttribute("in_Normal")->enable(mPassRenderLoop.getAttributeLocation("in_Normal"));


                mesh.renderElements();
                mesh.unbindBuffers();

                for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                {
                    multiTexObj.getBaseTextureAt(i)->unbind();
                }

            mPassRenderLoop.unbind();

        fboMPass->unbind();

        //SWAP
        GLuint temp = readBuffer;
        readBuffer = writeBuffer;
        writeBuffer = temp;

//        renderFbo(*fboMPass, quad, readBuffer);

        fboMPass->clearAttachment(writeBuffer);
        fboMPass->bindRenderBuffer(writeBuffer);
            mPassRenderLoop.bind();
                mPassRenderLoop.setUniform("projectionMatrix",  camera.getProjectionMatrix());
                mPassRenderLoop.setUniform("modelMatrix",       mesh.getModelMatrix());
                mPassRenderLoop.setUniform("viewMatrix",        camera.getViewMatrix());
                mPassRenderLoop.setUniform("lightViewMatrix",   lightTrackball.getViewMatrix());
                mPassRenderLoop.setUniform("firstPass",         true);
                mPassRenderLoop.setUniform("lastPass",          true);
                mPassRenderLoop.setUniform("multiPass",         true);
                mPassRenderLoop.setUniform("viewportSize", Eigen::Vector2f(viewPort_size[0], viewPort_size[1]));


                mPassRenderLoop.setUniform("lastPassTexture",   fboMPass->bindAttachment(readBuffer));

                mPassRenderLoop.setUniform("imageTexture_0", multiTexObj.getBaseTextureAt(1)->bind());


                glEnable(GL_DEPTH_TEST);
                Misc::errorCheckFunc(__FILE__, __LINE__);
                mesh.bindBuffers();
                Misc::errorCheckFunc(__FILE__, __LINE__);
                mesh.getAttribute("imageID_1")->enable(mPassRenderLoop.getAttributeLocation("in_coordText_0"));
                mesh.getAttribute("in_Position")->enable(mPassRenderLoop.getAttributeLocation("in_Position"));
                mesh.getAttribute("in_Normal")->enable(mPassRenderLoop.getAttributeLocation("in_Normal"));

                mesh.renderElements();
                mesh.unbindBuffers();

                for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                {
                    multiTexObj.getBaseTextureAt(i)->unbind();
                }

            mPassRenderLoop.unbind();

        fboMPass->unbind();

        //SWAP
        temp = readBuffer;
        readBuffer = writeBuffer;
        writeBuffer = temp;

        renderFbo(*fboMPass, quad, readBuffer);
    }
    void renderMultiPass(MultiTextureManagerObj& multiTexObj, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Mesh &mesh = *multiTexObj.getMesh();
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewPort_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewPort_size[0] * size, viewPort_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMPass->getWidth() != viewPort_size[0] || fboMPass->getHeight() != viewPort_size[1])
        {
            fboMPass->create(viewPort_size[0], viewPort_size[1], 2);
        }

        fboMPass->clearAttachments();

        bool multipass = true;
        bool lastpass = false;
        int totalTextures = multiTexObj.getNumPhotos();
        int limitPerPass = 4;
        int loops = totalTextures/limitPerPass;
        int resto = totalTextures%limitPerPass;

        glDisable (GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        int counter = loops;
        if(resto>0){
            counter++;
        }

        cout <<"Total number of textures.." << totalTextures << endl;
        cout <<"Texture limit per pass...." << limitPerPass << endl;
        cout <<"Number of loops..........." << loops << endl;
        cout <<"Texture on last pass......" << resto << endl;
        cout <<"" << endl;

//        counter = 1;
        while(counter--){
            int texturesPerPass = limitPerPass;
            if(lastpass && resto !=0) texturesPerPass = resto;

            fboMPass->clearAttachment(writeBuffer);
            fboMPass->bindRenderBuffer(writeBuffer);
                mPassRender.bind();

                    mPassRender.setUniform("projectionMatrix",camera.getProjectionMatrix());
                    mPassRender.setUniform("modelMatrix",     mesh.getModelMatrix());
                    mPassRender.setUniform("viewMatrix",      camera.getViewMatrix());
                    mPassRender.setUniform("lightViewMatrix", lightTrackball.getViewMatrix());
                    mPassRender.setUniform("firstPass",       true);
                    mPassRender.setUniform("lastPass",        lastpass);
                    mPassRender.setUniform("multiPass",       multipass);
                    mPassRender.setUniform("numImages",       texturesPerPass);

                    mPassRender.setUniform("viewportSize",    Eigen::Vector2f(viewPort_size[0], viewPort_size[1]));

                    mPassRender.setUniform("lastPassTexture", fboMPass->bindAttachment(readBuffer));


                    for(int i=0; i<texturesPerPass; i++){
                        string imageTexture = "imageTexture_" + std::to_string(i);
                        mPassRender.setUniform(imageTexture.c_str(),  multiTexObj.getBaseTextureAt(i * counter)->bind());
                    }
                    mesh.bindBuffers();

//                    glBindVertexArray(mesh.get_vao_id()); //Vertex Array Object
//                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.get_index_buffer_id());

                    mesh.getAttribute("in_Position")->enable(mPassRender.getAttributeLocation("in_Position"));
                    mesh.getAttribute("in_Normal")->enable(mPassRender.getAttributeLocation("in_Normal"));
                    for(int i=0; i<texturesPerPass; i++){
                        string imageID = "imageID_" + std::to_string(i * counter);
                        string in_coordText = "in_coordText_" + std::to_string(i);
                        mesh.getAttribute(imageID.c_str())->enable(mPassRender.getAttributeLocation(in_coordText.c_str()));
                    }

                    mesh.renderElements();
                    mesh.unbindBuffers();

                    for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                    {
                        multiTexObj.getBaseTextureAt(i)->unbind();
                    }
                mPassRender.unbind();
            fboMPass->unbind();

            //SWAP
            GLuint temp = readBuffer;
            readBuffer = writeBuffer;
            writeBuffer = temp;
            multipass = true;
            if(counter == 1)
            {
                lastpass = true;
            }
        }
        renderFbo(*fboMPass, quad, readBuffer);
    }

    void renderMultiPassLoop(MultiTextureManagerObj& multiTexObj, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Mesh &mesh = *multiTexObj.getMesh();
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewPort_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewPort_size[0] * size, viewPort_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMPass->getWidth() != viewPort_size[0] || fboMPass->getHeight() != viewPort_size[1])
        {
            fboMPass->create(viewPort_size[0], viewPort_size[1], 2);
        }

        fboMPass->clearAttachments();

        bool multipass = true;
        bool lastpass = false;
        int loops = multiTexObj.getNumPhotos();
        glDisable (GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        for(int i = 0; i <loops; i++){
            string imageID = "imageID_" + std::to_string(i);
            string imageTexture = "imageTexture_0";

            fboMPass->clearAttachment(writeBuffer);
            fboMPass->bindRenderBuffer(writeBuffer);
                mPassRenderLoop.bind();
                    mPassRenderLoop.setUniform("projectionMatrix",  camera.getProjectionMatrix());
                    mPassRenderLoop.setUniform("modelMatrix",       mesh.getModelMatrix());
                    mPassRenderLoop.setUniform("viewMatrix",        camera.getViewMatrix());
                    mPassRenderLoop.setUniform("lightViewMatrix",   lightTrackball.getViewMatrix());
                    mPassRenderLoop.setUniform("firstPass",         true);
                    mPassRenderLoop.setUniform("lastPass",          lastpass);
                    mPassRenderLoop.setUniform("multiPass",         multipass);

                    mPassRenderLoop.setUniform("viewportSize", Eigen::Vector2f(viewPort_size[0], viewPort_size[1]));

                    mPassRenderLoop.setUniform("lastPassTexture",   fboMPass->bindAttachment(readBuffer));

                    mPassRenderLoop.setUniform("imageTexture_0", multiTexObj.getBaseTextureAt(i)->bind());


                    mesh.bindBuffers();
//                    cout << imageID.c_str() << endl;
//                    cout << mesh.hasAttribute(imageID.c_str()) << endl;

                    mesh.getAttribute(imageID.c_str())->enable(mPassRenderLoop.getAttributeLocation("in_coordText_0"));
//                    mesh.getAttribute("in_Position")->enable(mPassRender.getAttributeLocation("in_Position"));
                    mesh.getAttribute("in_Normal")->enable(mPassRenderLoop.getAttributeLocation("in_Normal"));


                    mesh.renderElements();
                    mesh.unbindBuffers();

                    for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                    {
                        multiTexObj.getBaseTextureAt(i)->unbind();
                    }
                mPassRenderLoop.unbind();
            fboMPass->unbind();
            //SWAP
            GLuint temp = readBuffer;
            readBuffer = writeBuffer;
            writeBuffer = temp;
            multipass = true;
            if(i == loops-2)
            {
//                cout << "penultimo loop" << i << " " <<loops << endl;
                lastpass = true;
            }
//            fboMPass->saveAsPPM(string("novaMontagem" + std::to_string(i) + ".ppm").c_str(), readBuffer);
        }
//        fboMPass->saveAsPPM("nome.ppm", readBuffer);
        renderFbo(*fboMPass, quad, readBuffer);
    }
    Eigen::Vector4f getColorAt(Eigen::Vector2i v){
        return fboMPass->readPixel(readBuffer, v);
    }
    void saveImage(string &pathAndName){
        fboMPass->saveAsPPM(pathAndName, readBuffer);
    };
    Shader getPassRender() const;
    void setPassRender(const Shader &passRender);
};

}
#endif
