/*
 *  glc_colladaxmlelement.h
 *
 *  Created on: 22/12/2020
 *      Author: Laurent Ribon
 */
#ifndef GLC_COLLADAXMLELEMENT_H
#define GLC_COLLADAXMLELEMENT_H

#include <QString>

namespace ColladaElement {

const QString float3Element("float3");
const QString floatElement("float");

const QString idAttribute("id");
const QString sidAttribute("sid");
const QString nameAttribute("name");
const QString urlAttribute("url");
const QString refAttribute("ref");
const QString countAttribute("count");
const QString sourceAttribute("source");
const QString strideAttribute("stride");
const QString typeAttribute("type");
const QString semanticAttribute("semantic");
const QString setAttribute("set");
const QString inputSemanticAttribute("input_semantic");
const QString inputSetAttribute("input_set");
const QString textureAttribute("texture");
const QString textureCoordAttribute("texcoord");
const QString materialAttribute("material");
const QString offsetAttribute("offset");
const QString symbolAttribute("symbol");
const QString targetAttribute("target");
const QString meterAttribute("meter");

const QString colladaTopElement("COLLADA");
const QString contributorElement("contributor");
const QString authorElement("author");
const QString createdElement("created");
const QString modifiedElement("modified");
const QString upAxisElement("up_axis");
const QString unitElement("unit");

const QString authoringToolElement("authoring_tool");
const QString sourceDataElement("source_data");

const QString assetElement("asset");

const QString libraryImagesElement("library_images");
const QString initFromElement("init_from");
const QString inputElement("input");
const QString formatElement("format");

const QString libraryMaterialsElement("library_materials");
const QString materialElement("material");
const QString imageElement("image");
const QString instanceMaterialElement("instance_material");
const QString bindVertexInputElement("bind_vertex_input");
const QString effectElement("effect");
const QString instanceEffectElement("instance_effect");
const QString setParamElement("setparam");
const QString instanceNodeElement("instance_node");

const QString libraryEffectsElement("library_effects");
const QString profileCommonElement("profile_COMMON");
const QString newParamElement("newparam");
const QString surfaceElement("surface");
const QString sampler2DElement("sampler2D");
const QString sourceElement("source");
const QString minFilterElement("minfilter");
const QString magFilterElement("magfilter");
const QString phongElement("phong");
const QString emissionElement("emission");
const QString ambientElement("ambient");
const QString diffuseElement("diffuse");
const QString specularElement("specular");
const QString shininessElement("shininess");
const QString reflectiveElement("reflective");
const QString transparentElement("transparent");
const QString transparencyElement("transparency");
const QString colorElement("color");
const QString textureElement("texture");

const QString libraryGeometriesElement("library_geometries");
const QString geometryElement("geometry");
const QString meshElement("mesh");
const QString floatArrayElement("float_array");
const QString techniqueCommonElement("technique_common");
const QString techniqueElement("technique");
const QString accessorElement("accessor");
const QString paramElement("param");
const QString verticesElement("vertices");
const QString trianglesElement("triangles");
const QString linesElement("lines");
const QString linestripsElement("linestrips");
const QString bindMaterialElement("bind_material");
const QString primitiveElement("p");

const QString libraryVisualScenesElement("library_visual_scenes");
const QString visualSceneElement("visual_scene");
const QString nodeElement("node");
const QString instanceGeometryElement("instance_geometry");
const QString matrixElement("matrix");
const QString sceneElement("scene");
const QString instanceVisualSceneElement("instance_visual_scene");
const QString libraryNodesElement("library_nodes");




}
#endif // GLC_COLLADAXMLELEMENT_H
