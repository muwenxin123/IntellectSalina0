#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/ImageLayer>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <map3d/IEarth.h>
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>

#include <osg/Node>
#include <osg/Group>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Depth>
#include <osg/Uniform>
#include <osg/PolygonOffset>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "AgriMultispectralAnalysis.h"
#include "ui_AgriMultispectralAnalysis.h"
#include "QString"
#include "QFileDialog"
#include "QPushButton"
#include "QCString.h"
#include "QPixmap"
#include "AgriMultispectralSensorUtil.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osgEarth/GeoTransform>
#include <osgEarth/SpatialReference>

#include <osgManipulator/TranslateAxisDragger>
#include<osgManipulator/TabBoxDragger>
#include<osgManipulator/TrackballDragger>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osg/NodeVisitor>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>


//设置纹理着色
static void TextureShader(osg::ref_ptr<osg::Node> node)
{
	const char * vertexShader = {
		"void main(void ){\n"
		"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
		"}\n"
	};

	const char * fragShader = {
		"uniform sampler2D baseTexture;\n"
		"void main(void){\n"
		"   vec2 coord = gl_TexCoord[0].xy;\n"
		"   vec4 C = texture2D(baseTexture, coord)\n;"
		"	gl_FragColor = C;\n"
		"}\n"
	};

	osg::StateSet * ss = node->getOrCreateStateSet();
	osg::ref_ptr<osg::Program> program = new  osg::Program();
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);
}

//向场景中添加光源
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);//允许GL_LIGHT0光照
	stateset->setMode(GL_LIGHT1, osg::StateAttribute::ON);//允许GL_LIGHT1光照

	//创建两个个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	osg::ref_ptr<osg::Light> light1 = new osg::Light();
	//开启光照
	light->setLightNum(0);
	light1->setLightNum(1);//0对应GL_LIGHT0，1对应GL_LIGHT1，默认情况下为GL_LIGHT0
	//设置方向
	//light->setDirection(osg::Vec3(0.0f , 0.0f , -1.0f));
	//设置位置 最后一个参数1是点光源，0是平行光
	light->setPosition(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));//太阳光
	light1->setPosition(osg::Vec4(100.f, 100.f, 800.f, 1.0f));

	//设置光源颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setAmbient(osg::Vec4(0.3f, 0.3f, 0.5f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setDiffuse(osg::Vec4(0.75f, 0.75f, 0.75f, 1.0f));
	//镜面反射颜色
	light->setSpecular(osg::Vec4d(0.8f, 0.8f, 0.8f, 1.0f));
	light1->setSpecular(osg::Vec4d(0.8f, 0.8f, 0.8f, 1.0f));

	//设置恒衰减指数（可使用默认参数）
	light->setConstantAttenuation(1.f);
	light1->setConstantAttenuation(1.f);
	//设置线性衰减指数
	light->setLinearAttenuation(0.f);
	light1->setLinearAttenuation(0.f);
	//light->setLinearAttenuation(0.f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.f);
	light1->setQuadraticAttenuation(0.f);
	//stateset->setAttribute(light,osg::StateAttribute::OFF);

	//light->setQuadraticAttenuation(0.f);

	//创建光源 
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource();
	lightSource->setLight(light.get());
	lightSource1->setLight(light1.get());
	lightRoot->addChild(lightSource.get());
	lightRoot->addChild(lightSource1.get());

	return lightRoot.get();
}

// 递归加载所有 PagedLOD 的子节点
void preloadPagedLOD(osg::Node* node)
{
	if (!node) return;

	osg::PagedLOD* plod = dynamic_cast<osg::PagedLOD*>(node);
	if (plod)
	{
		for (unsigned int i = 0; i < plod->getNumFileNames(); ++i)
		{
			const std::string& fileName = plod->getFileName(i);
			if (!fileName.empty())
			{
				std::string filePath = plod->getDatabasePath() + fileName;

				QFileInfo fileInfo(QString::fromStdString(filePath));
				QString dirName = fileInfo.dir().dirName();
				QString baseName = fileInfo.completeBaseName();

				bool isEqual = (dirName == baseName);
				if (isEqual)
				{
					osg::ref_ptr<osg::Node> subNode = osgDB::readNodeFile(filePath);
					if (subNode.valid())
					{
#if 1 
						/* 方式一：模型的光照关闭（简单粗暴）*/
						subNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
#else
						/* 方式二：场景中使用光源，设置材质与法线*/
						subNode->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
						//TextureShader(subNode.get());

						osg::Material *material = new osg::Material;
						material->setDiffuse(osg::Material::FRONT, osg::Vec4(0.486, 0.486, 0.811, 1.0));
						material->setAmbient(osg::Material::FRONT, osg::Vec4(0.486, 0.486, 0.811, 1.0));
						material->setShininess(osg::Material::FRONT, 128.0);
						subNode->getOrCreateStateSet()->setAttribute(material);

						osg::ref_ptr<osg::Group> subGroup = createLight(subNode.get());
#endif
						plod->addChild(subNode); // 将子节点 attach 上，常驻内存
					}
				}
			}
		}
	}

	osg::Group* group = node->asGroup();
	if (group)
	{
		for (unsigned int i = 0; i < group->getNumChildren(); ++i)
		{
			preloadPagedLOD(group->getChild(i));
		}
	}
}

struct AgriMultispectralAnalysisPrivate
{
	AgriMultispectralAnalysisPrivate() 
		:_ImageLayer(nullptr)
	{
	};
	osg::ref_ptr<osgEarth::ImageLayer> _ImageLayer;
	std::vector<osg::ref_ptr<osg::Geode>> _GeodeVec;
};

AgriMultispectralAnalysis::AgriMultispectralAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriMultispectralAnalysis),
	_p(new AgriMultispectralAnalysisPrivate)
{
    ui->setupUi(this);

	//connect(ui->pushButton, &QPushButton::clicked, this, [&]() {
	//	QString inputImagePath = QFileDialog::getOpenFileName(this, tr2("请选择一张图片"), "", "Image Files(*.jpg, *.png, *.bmp, *.pgm, *.pbm);All(*.*)");
	//	if (inputImagePath.isEmpty()) {
	//		return;
	//	}
	//	QPixmap pm(inputImagePath);
	//	ui->label->setPixmap(pm);
	//});

	connect(ui->ptn_field_identify, &QPushButton::clicked, this, [&]() {

		if (m_pAgriFieldIdentifyDialog) {
			delete m_pAgriFieldIdentifyDialog;
		}
		m_pAgriFieldIdentifyDialog = new AgriFieldIdentifyDialog(this);
		m_pAgriFieldIdentifyDialog->move(550, 160);
		m_pAgriFieldIdentifyDialog->resize(QSize(800, 600));
		m_pAgriFieldIdentifyDialog->show(); 
	});

	connect(ui->ptn_analysis, &QPushButton::clicked, this, [&]() {

		if (m_pAgriDataAnalysisPredictDialog){
			delete m_pAgriDataAnalysisPredictDialog;
		}
		m_pAgriDataAnalysisPredictDialog = new AgriDataAnalysisPredictDialog(this);
		m_pAgriDataAnalysisPredictDialog->move(350, 160);
		m_pAgriDataAnalysisPredictDialog->resize(QSize(1200, 800));
		m_pAgriDataAnalysisPredictDialog->show();
	});

	connect(ui->ptn_onlineDownload, &QPushButton::clicked, this, [&]() {

		if (m_satelliteDataDialog){
			delete m_satelliteDataDialog;
			m_satelliteDataDialog = nullptr;
		}

		m_satelliteDataDialog = new AgriSatelliteDataDialog(this);
		if (m_satelliteDataDialog) m_satelliteDataDialog->show();
	});

	connect(ui->ptn_localImport, &QPushButton::clicked, this, [&]() {
		// TODO:
	});

	connect(ui->tree_widget, &AgriSatelliteDataTree::UpdateImageShow, this, [&](QScrollArea *scrollArea) {
		static bool isUpdate = false;
		if (!isUpdate && scrollArea)
		{
			isUpdate = true;
			ui->verticalLayout->addWidget(scrollArea); 
		}
	});

	connect(ui->tree_widget, &AgriSatelliteDataTree::LeafNodeClicked, this, [&](QTreeWidgetItem *item, int column) {
		
#if 0
		QTreeWidgetItem *parent = item->parent();
		if (parent) {
			QString imagePathSuffix = QString("%1/%2").arg(parent->text(0)).arg(item->text(0));

			// 三维图层展示
			osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
			if (osgEarth::Map *map = mapNode->getMap())
			{
				if (_p->_ImageLayer){
					map->removeImageLayer(_p->_ImageLayer);
				}

				// 设置GDAL图层
				osgEarth::Drivers::GDALOptions gdal;
				gdal.url() = AGRI_SENTINEL_IMAGE_PATH_PREFIX + imagePathSuffix.toStdString() + ".tif";

				_p->_ImageLayer = new osgEarth::ImageLayer("Multispectral_tiff", gdal);

				/*
				1.osgEarth 2.8 中，不能直接在代码里设置“透明值”，但可以通过 .earth 文件设置。
				<map>
				<image name="overlay">
				<driver>gdal</driver>
				<url>D:/Data/overlay_map.tif</url>
				<transparent>255,255,255</transparent>  <!-- 将白色设为透明 -->
				</image>
				</map>
				2.如果.tif 文件本身带有透明部分（例如使用 Alpha 通道或 NoData 值），osgEarth 会自动处理。
				*/
				_p->_ImageLayer->setOpacity(0.99);
				map->addImageLayer(_p->_ImageLayer); // 叠加顺序即渲染顺序
			}
		}
#else
#endif
	});

	connect(ui->tree_widget, &AgriSatelliteDataTree::UpdateCheckedImages, this, [&](std::vector<AgriSatelliteDataTree::ImageInfo>& imageVec) {

		// 图例展示
		ui->compare_analysis->UpdateShowLegend(imageVec);

		/*
		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		for (const auto& geode : _p->_GeodeVec){
			if (geode){
				mapNode->removeChild(geode);
			}
		}
		_p->_GeodeVec.clear();
		
		// 设置浮空高度（以米为单位）
		double relativeHeight = 120.0;

		for (const auto& imageInfo : imageVec)
		{
			// 加载影像 (AGRI_SENTINEL_IMAGE_PATH_PREFIX+)
			std::string tifPath =  imageInfo.imagePath.toStdString();
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(tifPath);
			if (!image)
			{
				std::cerr << "无法加载影像文件: " << tifPath << std::endl;
				return 1;
			}

			// 假设你已知影像的地理范围（以 WGS84 度为单位）
			// 或者可以手动指定，比如左下角经纬度：
			double lon_min = 116.19475710000000;
			double lat_min = 40.262845900000002;
			double lon_max = 116.19747670000000;
			double lat_max = 40.265551500000001;

			// 创建 SRS（WGS84 地理坐标系）
			osgEarth::SpatialReference* srs = osgEarth::SpatialReference::get("wgs84");

			// 将四角地理坐标转换为地心坐标（ECEF）
			osg::Vec3d ll, lr, ur, ul;
			srs->getEllipsoid()->convertLatLongHeightToXYZ(
				osg::DegreesToRadians(lat_min), osg::DegreesToRadians(lon_min), relativeHeight, ll.x(), ll.y(), ll.z());
			srs->getEllipsoid()->convertLatLongHeightToXYZ(
				osg::DegreesToRadians(lat_min), osg::DegreesToRadians(lon_max), relativeHeight, lr.x(), lr.y(), lr.z());
			srs->getEllipsoid()->convertLatLongHeightToXYZ(
				osg::DegreesToRadians(lat_max), osg::DegreesToRadians(lon_max), relativeHeight, ur.x(), ur.y(), ur.z());
			srs->getEllipsoid()->convertLatLongHeightToXYZ(
				osg::DegreesToRadians(lat_max), osg::DegreesToRadians(lon_min), relativeHeight, ul.x(), ul.y(), ul.z());

			// 构建纹理四边形
			osg::ref_ptr<osg::Geometry> quad = new osg::Geometry();
			osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
			verts->push_back(ll);
			verts->push_back(lr);
			verts->push_back(ur);
			verts->push_back(ul);
			quad->setVertexArray(verts);

			osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
			texcoords->push_back(osg::Vec2(0.0f, 0.0f));
			texcoords->push_back(osg::Vec2(1.0f, 0.0f));
			texcoords->push_back(osg::Vec2(1.0f, 1.0f));
			texcoords->push_back(osg::Vec2(0.0f, 1.0f));
			quad->setTexCoordArray(0, texcoords);

			osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
			indices->push_back(0);
			indices->push_back(1);
			indices->push_back(2);
			indices->push_back(3);
			quad->addPrimitiveSet(indices);

			// 应用纹理
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
			texture->setResizeNonPowerOfTwoHint(false);

			osg::ref_ptr<osg::StateSet> stateset = quad->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

			// 启用混合以支持透明显示
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			// 关闭光照效果
			stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

			// 添加到 Geode
			osg::ref_ptr<osg::Geode> geode = new osg::Geode();
			geode->addDrawable(quad);
			geode->getOrCreateStateSet()->setRenderBinDetails(10, "RenderBin");

			// 三维图层展示
			mapNode->addChild(geode); 

			relativeHeight += 25.0;
			_p->_GeodeVec.emplace_back(geode);
		}
		*/
	});

	/* 生成金字塔顶层Data.osgb文件 */  
	// AgriMultispectralSensorUtil::CreateObliqueIndexes("../data/maps_sichuan_chengdu_pengzhou/OSGB/Production_osgb");

	/* 配置所有顶层.osgb文件到指定.earth文件
	QString tilesDir = "../data/maps_beijing_changping_shisanling/OSGB/Production/Data";
	QString earthFile = "../data/lyEarth/earth/lyEarth_flat.earth";
	QString urlPrefix = "../data/maps_beijing_changping_shisanling/OSGB/Production/Data";

	AgriMultispectralSensorUtil::AddModelsToEarth(tilesDir, earthFile, urlPrefix);
	*/
	 
	{
		
		//osg::ref_ptr<osg::Group> group = new osg::Group();

		//// 0. 加载本地模型
		//osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/11.osgb");
		//if (!model)
		//{
		//	osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
		//}

		//osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		//// 1. 获取地理变换矩阵（根据经纬度 + 高程）
		//osg::Matrix posMatrix;
		//osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		//em->computeLocalToWorldTransformFromLatLongHeight(
		//	osg::inDegrees(40.2621733), osg::inDegrees(116.1993307), 20, posMatrix);

		//// 2. 创建缩放矩阵 （OSG矩阵乘法是“从右到左”应用变换）
		//double scale = 1; // 0.006;
		//osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale); // 先把模型移动到地理坐标，然后在该点周围进行缩放（即以地理点为中心缩放）

		//osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(210.0), osg::Z_AXIS);

		//// 3. 合并矩阵（先缩放再变换）
		//osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		//// 4. 包裹模型
		//osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		//transform->setMatrix(finalMatrix);
		//transform->addChild(model.get());

		//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		//// 5. 添加到场景根节点
		//group->addChild(transform.get());
		//
		//mapNode->addChild(group);
		
	}

	{
		/* 预加载所有分页节点
		osg::ref_ptr<osg::Group> group = new osg::Group();

		// 加载一个带 PagedLOD 的场景（如 .earth、.osgb、.ive 等）
		osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/Production/Data.osgb"); 
		if (root.valid()){
			preloadPagedLOD(root.get());
		}

		// 5. 添加到场景根节点
		group->addChild(root.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		mapNode->addChild(group);
		*/
	}

	{
		osg::ref_ptr<osg::Group> group = new osg::Group();

		// 0. 加载本地模型
		//osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/Production_osgb/Data.osgb");
		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/Production_osgb/Data.osgb");
		if (!model)
		{
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
		}

		// 1. 预加载所有子模型
		preloadPagedLOD(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		// 2. 获取地理变换矩阵（根据经纬度 + 高程）
		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			//osg::inDegrees(30.94775), osg::inDegrees(104.089575), 1, posMatrix);
			osg::inDegrees(40.26387), osg::inDegrees(116.19766), 15, posMatrix);

		// 3. 包裹模型
		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(posMatrix);
		transform->addChild(model.get());

		// 4. 添加到场景根节点
		group->addChild(transform.get());

		mapNode->addChild(group); 
	}

	{
		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/build.osgb");
		if (!model) {
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
			return;
		}

		//model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(30.94787), osg::inDegrees(104.08895), 526, posMatrix);

		double scale = 1.0;
		osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
		osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(-2.0), osg::Z_AXIS);
		osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(finalMatrix);
		transform->addChild(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

#if 0

		osg::ref_ptr<osg::Group> groupSensor = new osg::Group;;

		osg::ref_ptr<osgManipulator::Selection> selection = new osgManipulator::Selection;
		selection->addChild(transform);

		scale = transform->getBound().radius() * 1.6;

		// 盒式拖拽器 （由六个平面拖拽器构成，可在各个面上进行缩放、平移）
		//osgManipulator::TabBoxDragger* tad = new osgManipulator::TabBoxDragger();
		//tad->setupDefaultGeometry();//使用默认的图形，即一个立方体
		//tad->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
		//tad->addTransformUpdating(selection);//Dragger要改变的对象。
		//tad->setHandleEvents(true);

		// 旋转拖拽器
		osgManipulator::TrackballDragger* dragger = new osgManipulator::TrackballDragger();
		dragger->setupDefaultGeometry();
		dragger->setMatrix(osg::Matrix::scale(scale*0.5, scale*0.5, scale*0.5)*osg::Matrix::translate(transform->getBound().center()));
		dragger->addTransformUpdating(selection);
		dragger->setHandleEvents(true);

		// 三维平移拖拽器（可在三个方向上对模型进行拖拽）
		osgManipulator::TranslateAxisDragger * move3d = new osgManipulator::TranslateAxisDragger();
		move3d->setupDefaultGeometry();
		move3d->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
		move3d->addTransformUpdating(selection);
		move3d->setHandleEvents(true);

		//// 旋转圆柱拖拽器
		//osgManipulator::RotateCylinderDragger *rote = new osgManipulator::RotateCylinderDragger();
		//rote->setupDefaultGeometry();
		//rote->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
		//rote->addTransformUpdating(selection);
		//rote->setHandleEvents(true);

		// CommandManager管理命令，可用于撤销和重做操作
		osg::ref_ptr<osgManipulator::CommandManager> manager = new osgManipulator::CommandManager;
		manager->connect(*dragger, *selection);

		groupSensor->addChild(selection);
		//groupSensor->addChild(tad);
		groupSensor->addChild(dragger);
		groupSensor->addChild(move3d);
		//groupSensor->addChild(rote);

		mapNode->addChild(groupSensor);
#else

		osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
		lightRoot->addChild(transform);

		//开启光照
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
		stateset = lightRoot->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

		//计算包围盒
		osg::BoundingSphere bs;
		transform->computeBound();
		bs = transform->getBound();

		//创建一个Light对象
		osg::ref_ptr<osg::Light> light = new osg::Light();
		light->setLightNum(0);
		//设置方向
		light->setDirection(osg::Vec3(0.0f, 1.0f, -1.0f));
		//设置位置
		light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
		//设置环境光的颜色
		light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//设置散射光颜色
		light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//设置恒衰减指数
		light->setConstantAttenuation(0.5f);
		//设置线形衰减指数
		light->setLinearAttenuation(0.0f);
		//设置二次方衰减指数
		light->setQuadraticAttenuation(0.0f);

		//创建光源
		osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
		lightSource->setLight(light);

		lightRoot->addChild(lightSource);

		mapNode->addChild(lightRoot);

#endif
	}

	{
		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/trees.osgb");
		if (!model) {
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
			return;
		}

		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(30.94767), osg::inDegrees(104.08921), 523, posMatrix);

		double scale = 1.0;
		osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
		osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(0.0), osg::Z_AXIS);
		osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(finalMatrix);
		transform->addChild(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

#if 1

		//关闭光照
		model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		mapNode->addChild(transform);

#else

		osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
		lightRoot->addChild(transform);

		//开启光照
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
		stateset = lightRoot->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

		//计算包围盒
		osg::BoundingSphere bs;
		transform->computeBound();
		bs = transform->getBound();

		//创建一个Light对象
		osg::ref_ptr<osg::Light> light = new osg::Light();
		light->setLightNum(0);
		//设置方向
		light->setDirection(osg::Vec3(0.0f, 1.0f, -1.0f));
		//设置位置
		light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
		//设置环境光的颜色
		light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//设置散射光颜色
		light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//设置恒衰减指数
		light->setConstantAttenuation(1.0f);
		//设置线形衰减指数
		light->setLinearAttenuation(0.0f);
		//设置二次方衰减指数
		light->setQuadraticAttenuation(0.0f);

		//创建光源
		osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
		lightSource->setLight(light);

		lightRoot->addChild(lightSource);

		mapNode->addChild(lightRoot);

#endif
	}

	{
		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/UCV.ive");
		if (!model) {
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
			return;
		}

		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(30.9469604), osg::inDegrees(104.0901813), 515.5f, posMatrix);

		double scale = 5.0;
		osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
		osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(0.0), osg::Z_AXIS);
		osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(finalMatrix);
		transform->addChild(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		//关闭光照
		//model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		mapNode->addChild(transform);
	}

	{
		std::vector<std::string> modelPath = {
			"../data/maps_sichuan_chengdu_pengzhou/OSGB/UCV.ive",
			"../data/maps_sichuan_chengdu_pengzhou/OSGB/UAV.ive",
			"../data/maps_sichuan_chengdu_pengzhou/OSGB/Maize.ive",
		};

		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/UAV.ive");
		if (!model) {
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
			return;
		}

		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(30.9475587), osg::inDegrees(104.0898500), 515.5f, posMatrix);

		double scale = 5.0;
		osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
		osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(0.0), osg::Z_AXIS);
		osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(finalMatrix);
		transform->addChild(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		//关闭光照
		//model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		mapNode->addChild(transform);
	}

	{
		/*
		osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_sichuan_chengdu_pengzhou/OSGB/Maize.osgb");
		if (!model) {
			osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
			return;
		}

		osg::Matrix posMatrix;
		osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(30.9473947), osg::inDegrees(104.0893489), 516.5f, posMatrix);

		double scale = 5.0;
		osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
		osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(0.0), osg::Z_AXIS);
		osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(finalMatrix);
		transform->addChild(model.get());

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		//关闭光照
		//model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		mapNode->addChild(transform);
		*/
	}
}

AgriMultispectralAnalysis::~AgriMultispectralAnalysis()
{
    delete ui;
}

AgriFieldIdentifyDialog* AgriMultispectralAnalysis::getFieldIdentifyDialog()
{
	return m_pAgriFieldIdentifyDialog;
}