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

#ifndef __MULTIMASK__
#define __MULTIMASK__

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
class MultiMask : public Effect
{

private:

    /// Phong Shader
    Shader phong_shader;
    Shader depthMap;
    Shader maskPass;
    Shader maskFusePass;
    Shader maskAnglePass;
    Shader maskDepthPass;
    Shader maskBorderPass;
    Shader multiTF;
    Shader mPassRender;

    Shader showFBO;

    /// Default color
    Eigen::Vector4f default_color;

    Framebuffer *fboDepthMap;
    Framebuffer *fboMPass;
    Framebuffer *fboMask;

    Framebuffer *fboMasksFused;
    Framebuffer *fboMaskAngle;
    Framebuffer *fboMaskDepth;
    Framebuffer *fboMaskBorder;

    Framebuffer *currentFBO;

    vector<Framebuffer *> maskList;

    Mesh quad;

    GLuint writeBuffer;
    GLuint readBuffer;
    GLuint currentBuffer;

    int ID_DepthTextureNorm;
    int ID_DepthTextureNonNorm;

    int ID_MaskAngle;
    int ID_MaskDepth;
    int ID_MaskBorder;



public:
    bool firstRenderFlag;
    /**
     * @brief Default constructor.
     */
    MultiMask (void)
    {
        default_color << 0.7, 0.7, 0.7, 1.0;
        fboDepthMap =0;
        fboMPass = 0;
        fboMask = 0;

        ID_DepthTextureNorm = 0;
        ID_DepthTextureNonNorm = 1;

        ID_MaskAngle = 0;
        ID_MaskDepth = 0;
        ID_MaskBorder = 0;

        writeBuffer = 0;
        readBuffer = 1;
    }

    /**
     * @brief Default destructor
     */
    virtual ~MultiMask (void) {
        delete fboDepthMap;
        delete fboMPass;
        delete fboMask;
        delete fboMaskDepth;
        delete fboMaskBorder;
        delete fboMaskAngle;
        delete currentFBO;
        delete fboMasksFused;
    }

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
//        loadShader(depthMap,        "nonnormdepthmap");

        loadShader(depthMap,        "depthmap");
        loadShader(phong_shader,    "phongshader");
        loadShader(mPassRender,     "multipass");
        loadShader(showFBO,         "showFbo");
        loadShader(maskPass,        "maskpass");
        loadShader(maskFusePass,    "maskfusepass");
        loadShader(maskAnglePass,   "maskanglepass");
        loadShader(maskDepthPass,   "maskdepthpass");
        loadShader(maskBorderPass, "maskborderpass");

        //Initialize multiTF using coordtf shaders
        multiTF.load("coordtf", shaders_dir);
        const char * vars[] = {"nCoord"};
        multiTF.initializeTF(1,vars);
        shaders_list.push_back(&multiTF);

        fboDepthMap     = new Framebuffer();
        fboMPass        = new Framebuffer();
        fboMask         = new Framebuffer();
        fboMaskAngle    = new Framebuffer();
        fboMasksFused   = new Framebuffer();
        fboMaskDepth    = new Framebuffer();
        fboMaskBorder   = new Framebuffer();

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

        if(fboDepthMap->getWidth() != viewport_size[0] || fboDepthMap->getHeight() != viewport_size[1])
        {
            fboDepthMap->create(viewport_size[0], viewport_size[1], 2);
        }

        fboDepthMap->clearAttachments();
//        fboDepthMap->bindRenderBuffer(ID_DepthTextureNorm);
        fboDepthMap->bindRenderBuffers(ID_DepthTextureNorm, ID_DepthTextureNonNorm);
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

        fboDepthMap->unbind();
        fboDepthMap->clearDepth();
    }
    void prepareMaskPass(Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {

        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMask->getWidth() != viewport_size[0] || fboMask->getHeight() != viewport_size[1])
        {
            fboMask->create(viewport_size[0], viewport_size[1], 3);
        }

        fboMask->clearAttachments();
        fboMask->bindRenderBuffers(ID_MaskAngle, ID_MaskDepth, ID_MaskBorder);
//          cout << viewport_size << endl;
            maskPass.bind();
            maskPass.setUniform("projectionMatrix", camera.getProjectionMatrix());
            maskPass.setUniform("modelMatrix", mesh.getModelMatrix());
            maskPass.setUniform("viewMatrix", camera.getViewMatrix());
            maskPass.setUniform("near", 0.1);
            maskPass.setUniform("far", 10000.0);
            maskPass.setUniform("in_depthmap", fboDepthMap->bindAttachment(0));
            maskPass.setUniform("viewportSize",    Eigen::Vector2f(774.0, 518.0));
            mesh.setAttributeLocation(maskPass);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            maskPass.unbind();

        fboMask->unbind();
        fboMask->clearDepth();
        renderFbo(*fboMask, quad, ID_MaskDepth);
    }

    void prepareMaskAnglePass(Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {

        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMaskAngle->getWidth() != viewport_size[0] || fboMaskAngle->getHeight() != viewport_size[1])
        {
            fboMaskAngle->create(viewport_size[0], viewport_size[1], 1);
        }

        fboMaskAngle->clearAttachments();
        fboMaskAngle->bindRenderBuffer(ID_MaskAngle);

            maskAnglePass.bind();
            maskAnglePass.setUniform("projectionMatrix", camera.getProjectionMatrix());
            maskAnglePass.setUniform("modelMatrix", mesh.getModelMatrix());
            maskAnglePass.setUniform("viewMatrix", camera.getViewMatrix());

            mesh.setAttributeLocation(maskAnglePass);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            maskAnglePass.unbind();

        fboMaskAngle->unbind();
        fboMaskAngle->clearDepth();
//        renderFbo(*fboMaskAngle, quad);
    }
    void prepareMaskDepthPass(Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        vector<GLfloat> pixels;
        fboDepthMap->readBuffer(ID_DepthTextureNonNorm, pixels);

        //Get min and max from depthMaps
        GLfloat minN = 0;
        GLfloat maxN = 0;
        for(int i = 0; i < pixels.size(); i+=4)
        {
            if(pixels.at(i)< minN) minN = pixels.at(i);
            if(pixels.at(i)> maxN) maxN = pixels.at(i);
        }

        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
        //create frame buffers
        if(fboMaskDepth->getWidth() != viewport_size[0] || fboMaskDepth->getHeight() != viewport_size[1])
        {
            fboMaskDepth->create(viewport_size[0], viewport_size[1], 1);
        }

        //render image
        fboMaskDepth->clearAttachments();
        fboMaskDepth->bindRenderBuffer(0);

            maskDepthPass.bind();
            maskDepthPass.setUniform("projectionMatrix", camera.getProjectionMatrix());
            maskDepthPass.setUniform("modelMatrix", mesh.getModelMatrix());
            maskDepthPass.setUniform("viewMatrix", camera.getViewMatrix());
            maskDepthPass.setUniform("minDepth", minN);
            maskDepthPass.setUniform("maxDepth", maxN);
            maskDepthPass.setUniform("depthMap", fboDepthMap->bindAttachment(ID_DepthTextureNonNorm));
            maskDepthPass.setUniform("viewportSize",  Eigen::Vector2f(774.0, 518.0));

            mesh.setAttributeLocation(maskDepthPass);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            maskDepthPass.unbind();

        fboMaskDepth->unbind();
        fboMaskDepth->clearDepth();
    }
    void prepareMaskBorderPass(Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMaskBorder->getWidth() != viewport_size[0] || fboMaskBorder->getHeight() != viewport_size[1])
        {
            fboMaskBorder->create(viewport_size[0], viewport_size[1], 1);
        }

        fboMaskBorder->clearAttachments();
        fboMaskBorder->bindRenderBuffer(ID_MaskBorder);

            maskBorderPass.bind();
            maskBorderPass.setUniform("projectionMatrix", camera.getProjectionMatrix());
            maskBorderPass.setUniform("modelMatrix", mesh.getModelMatrix());
            maskBorderPass.setUniform("viewMatrix", camera.getViewMatrix());
            maskBorderPass.setUniform("depthMap", fboDepthMap->bindAttachment(ID_DepthTextureNonNorm));
            maskBorderPass.setUniform("viewportSize",  Eigen::Vector2f(774.0, 518.0));

            mesh.setAttributeLocation(maskBorderPass);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            maskBorderPass.unbind();

        fboMaskBorder->unbind();
        fboMaskBorder->clearDepth();

        renderFbo(*fboMaskBorder, quad);
    }
    void fuseMasks(Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& lightTrackball)
    {
        fboMasksFused = new Framebuffer();
        Eigen::Vector4f viewPort = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        int size = 1;
        viewPort << 0, 0, viewport_size[0] * size, viewport_size[1] * size;
        glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);

        if(fboMasksFused->getWidth() != viewport_size[0] || fboMasksFused->getHeight() != viewport_size[1])
        {
            fboMasksFused->create(viewport_size[0], viewport_size[1], 1);
        }

        fboMasksFused->clearAttachments();
        fboMasksFused->bindRenderBuffer(0);
            maskFusePass.bind();
            maskFusePass.setUniform("projectionMatrix", camera.getProjectionMatrix());
            maskFusePass.setUniform("modelMatrix", mesh.getModelMatrix());
            maskFusePass.setUniform("viewMatrix", camera.getViewMatrix());
            maskFusePass.setUniform("viewportSize",    Eigen::Vector2f(viewport_size[0], viewport_size[1]));
            maskFusePass.setUniform("angleMask",    fboMaskAngle->bindAttachment(ID_MaskAngle));
            maskFusePass.setUniform("depthMask",    fboMaskDepth->bindAttachment(0));

            mesh.setAttributeLocation(maskFusePass);

            glEnable(GL_DEPTH_TEST);
            mesh.render();
            maskFusePass.unbind();

        fboMasksFused->unbind();
        fboMasksFused->clearDepth();

//        renderFbo(*fboMasksFused, quad);
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
                multiTF.setUniform("depthMapTexture", fboDepthMap->bindAttachment(ID_DepthTextureNorm));
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
        fboDepthMap->unbindAttachments();
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
        currentFBO = &renderFBO;
        currentBuffer = renderBuffer;
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
//        if(firstRenderFlag)
        {
            Tucano::Camera cam;
            cam.setViewport(camera.getViewport());
            int total = multiTextObj.getNumPhotos();
            total = 1;
            for(int i = 0; i < total ; i++){
                multiTextObj.calibrateCamera(cam);
                depthMapRender(*multiTextObj.getMesh(), cam, lightTrackball);

                prepareMaskAnglePass(*multiTextObj.getMesh(), cam, lightTrackball);
                prepareMaskDepthPass(*multiTextObj.getMesh(), cam, lightTrackball);
                prepareMaskBorderPass(*multiTextObj.getMesh(), cam, lightTrackball);

                fuseMasks(*multiTextObj.getMesh(), cam, lightTrackball);

                maskList.push_back(fboMasksFused);

                updateTF(multiTextObj, cam, lightTrackball);
//                multiTextObj.nextPhoto();
            }
            multiTextObj.changePhotoReferenceTo(0);
            firstRenderFlag = false;
         }
//        renderMultiPass(multiTextObj, camera, lightTrackball);
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


//        cout <<"Total number of textures.." << totalTextures << endl;
//        cout <<"Texture limit per pass...." << limitPerPass << endl;
//        cout <<"Number of loops..........." << loops << endl;
//        cout <<"Texture on last pass......" << resto << endl;
//        cout <<"" << endl;

//        counter = 1;
//        limitPerPass = 2;
        loops = counter;
        counter = 0;
        while(counter < loops)
        {
            if(counter == (loops - 1))
            {
                lastpass = true;
            }
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

                    mPassRender.setUniform("prevPassTexture", fboMPass->bindAttachment(readBuffer));

//                    cout <<  texturesPerPass << endl;
//                    texturesPerPass = 4;
                    for(int i=0; i<texturesPerPass; i++){
                        string imageTexture = "imageTexture_" + std::to_string(i);
                        string maskTexture = "mask_" + std::to_string(i);
                        mPassRender.setUniform(imageTexture.c_str(),  multiTexObj.getBaseTextureAt(i + (counter * (limitPerPass)))->bind());
                        mPassRender.setUniform(maskTexture.c_str(),  maskList.at(i + (counter * (limitPerPass)))->bindAttachment(0));

                    }

                    mesh.bindBuffers();

                    mesh.getAttribute("in_Position")->enable(mPassRender.getAttributeLocation("in_Position"));
                    mesh.getAttribute("in_Normal")->enable(mPassRender.getAttributeLocation("in_Normal"));

                    for(int i=0; i< texturesPerPass; i++){
                        string imageID = "imageID_" + std::to_string(i + (counter * (limitPerPass)));
                        string in_coordText = "in_coordText_" + std::to_string(i);
                        mesh.getAttribute(imageID.c_str())->enable(mPassRender.getAttributeLocation(in_coordText.c_str()));
                    }

                    mesh.renderElements();
                    mesh.unbindBuffers();

                    for(int i =0; i < multiTexObj.getNumPhotos(); i++)
                    {
                        multiTexObj.getBaseTextureAt(i)->unbind();
                        maskList.at(i)->unbind();
                    }
                mPassRender.unbind();
            fboMPass->unbind();

            //SWAP
            GLuint temp = readBuffer;
            readBuffer = writeBuffer;
            writeBuffer = temp;

            counter++;
        }
        renderFbo(*fboMPass, quad, readBuffer);
    }

    Eigen::Vector4f getColorAt(Eigen::Vector2i v){
        return currentFBO->readPixel(currentBuffer, v);
    }
    void saveImage(string &pathAndName){
        currentFBO->saveAsPPM(pathAndName, currentBuffer);
    };
};

}
#endif
