#include "drawAble_drawFire.h"

#include <osg/Group>
#include <osg/Geode>

#include <osg/MatrixTransform>

#include <osgParticle/Particle>
#include <osgParticle/LinearInterpolator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/RadialShooter>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/PointPlacer>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>

void createFireBall(osg::MatrixTransform* smokeNode, float _size)
{

	osgParticle::Particle particleTempalte;
	particleTempalte.setShape(osgParticle::Particle::HEXAGON);
	particleTempalte.setLifeTime(5);
	particleTempalte.setSizeRange(osgParticle::rangef(_size, _size));
	particleTempalte.setAlphaRange(osgParticle::rangef(1, 0));
	particleTempalte.setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.2f, 0.0f, 1.0f),
		osg::Vec4(0.1f, 0.1f, 0.1f, 0)
	));

	particleTempalte.setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	particleTempalte.setVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));
	particleTempalte.setMass(0.5f);
	particleTempalte.setRadius(0.2f);
	particleTempalte.setSizeInterpolator(new osgParticle::LinearInterpolator);
	particleTempalte.setAlphaInterpolator(new osgParticle::LinearInterpolator);
	particleTempalte.setColorInterpolator(new osgParticle::LinearInterpolator);

	osgParticle::ParticleSystem* particleSystem = new osgParticle::ParticleSystem;
	particleSystem->setDataVariance(osg::Node::STATIC);

	particleSystem->setDefaultAttributes("../data/images/smoke.rgb", true, false);
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(particleSystem);
	smokeNode->addChild(geode);

	particleSystem->setDefaultParticleTemplate(particleTempalte);

	osgParticle::RandomRateCounter* particleGenerateRate = new osgParticle::RandomRateCounter();
	particleGenerateRate->setRateRange(30, 50);

	particleGenerateRate->setDataVariance(osg::Node::DYNAMIC);

	osgParticle::PointPlacer* particlePlacer = new osgParticle::PointPlacer;

	particlePlacer->setCenter(osg::Vec3(0.0f, 0.0f, 0.0f));
	particlePlacer->setDataVariance(osg::Node::DYNAMIC);

	osgParticle::RadialShooter* particleShooter = new osgParticle::RadialShooter;

	particleShooter->setDataVariance(osg::Node::DYNAMIC);
	particleShooter->setThetaRange(-0.1f, 0.1f);

	particleShooter->setPhiRange(-0.1f, 0.1f);
	particleShooter->setInitialSpeedRange(_size / 2.0, _size * 5 / 2.0);

	osgParticle::ModularEmitter* emitter = new osgParticle::ModularEmitter;
	emitter->setDataVariance(osg::Node::DYNAMIC);
	emitter->setCullingActive(false);

	emitter->setParticleSystem(particleSystem);

	emitter->setCounter(particleGenerateRate);

	emitter->setPlacer(particlePlacer);

	emitter->setShooter(particleShooter);

	smokeNode->addChild(emitter);

	osgParticle::ParticleSystemUpdater* particleSystemUpdater = new osgParticle::ParticleSystemUpdater;

	particleSystemUpdater->addParticleSystem(particleSystem);

	smokeNode->addChild(particleSystemUpdater);

	osgParticle::ModularProgram* particleMoveProgram = new osgParticle::ModularProgram;
	particleMoveProgram->setParticleSystem(particleSystem);

	smokeNode->addChild(particleMoveProgram);
}