/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "ctl_cs_plugin.h"

#include <kcomponentdata.h>
#include <kgenericfactory.h>
#include <KoColorSpaceRegistry.h>
#include "KoBasicHistogramProducers.h"
#include <KoCtlColorProfile.h>
#include <kstandarddirs.h>

#include <OpenCTL/ModulesManager.h>
#include "kis_debug.h"
#include "KoCtlColorSpaceInfo.h"
#include "KoCtlColorSpaceFactory.h"

// #include "kis_lms_f32_colorspace.h"

typedef KGenericFactory<CTLCSPlugin> CTLCSPluginPluginFactory;
K_EXPORT_COMPONENT_FACTORY(krita_ctlcs_plugin, CTLCSPluginPluginFactory("krita"))


CTLCSPlugin::CTLCSPlugin(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{

    KoColorSpaceRegistry * f = KoColorSpaceRegistry::instance();
    {
        // Set PigmentCMS's ctl module directory
        QStringList ctlModulesDirs = KGlobal::mainComponent().dirs()->findDirs( "data", "pigmentcms/ctlmodules/");
        dbgPlugins << ctlModulesDirs;
        foreach(const QString & dir, ctlModulesDirs)
        {
            dbgPlugins << "Append : " << dir << " to the list of CTL modules";
            OpenCTL::ModulesManager::instance()->addDirectory( dir.toAscii().data());
        }
        
        // Load CTL Profiles
        KGlobal::mainComponent().dirs()->addResourceType("ctl_profiles", "data", "pigmentcms/ctlprofiles/");
        QStringList ctlprofileFilenames;
        ctlprofileFilenames += KGlobal::mainComponent().dirs()->findAllResources("ctl_profiles", "*.ctlp",  KStandardDirs::Recursive);
        dbgPlugins << "There are " << ctlprofileFilenames.size() << " CTL profiles";
        if (!ctlprofileFilenames.empty()) {
            KoColorProfile* profile = 0;
            for( QStringList::Iterator it = ctlprofileFilenames.begin(); it != ctlprofileFilenames.end(); ++it ) {
                dbgPlugins << "Load profile : " << *it;
                profile = new KoCtlColorProfile(*it);
                profile->load();
                if (profile->valid()) {
                    dbgPlugins << "Valid profile : " << profile->name();
                    f->addProfile( profile );
                } else {
                    dbgPlugins << "Invalid profile : " << profile->name();
                    delete profile;
                }
            }
        }
        
        // Load CTL Color spaces
        KGlobal::mainComponent().dirs()->addResourceType("ctl_colorspaces", "data", "pigmentcms/ctlcolorspaces/");
        QStringList ctlcolorspacesFilenames;
        ctlcolorspacesFilenames += KGlobal::mainComponent().dirs()->findAllResources("ctl_colorspaces", "*.ctlcs",  KStandardDirs::Recursive);
        dbgPlugins << "There are " << ctlcolorspacesFilenames.size() << " CTL colorspaces";
        if (!ctlcolorspacesFilenames.empty()) {
            for( QStringList::Iterator it = ctlcolorspacesFilenames.begin(); it != ctlcolorspacesFilenames.end(); ++it ) {
                dbgPlugins << "Load colorspace : " << *it;
                KoCtlColorSpaceInfo* info = new KoCtlColorSpaceInfo(*it);
                if(info->load())
                {
                    f->add(new KoCtlColorSpaceFactory(info));
                } else {
                    dbgPlugins << "Invalid color space : " << *it;
                    delete info;
                }
            }
        }
#if 0
        KisLmsAF32ColorSpaceFactory * csf  = new KisLmsAF32ColorSpaceFactory();
        f->add(csf);
        const KoCtlColorProfile* profile = static_cast<const KoCtlColorProfile*>(f->profileByName(csf->defaultProfile()));
        Q_ASSERT(profile);
        KoColorSpace * colorSpaceLMSF32  = new KisLmsAF32ColorSpace(profile);

        KoHistogramProducerFactoryRegistry::instance()->add(
            new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>
            (KoID("LMSF32HISTO", i18n("Float32 Histogram")), colorSpaceLMSF32));
#endif
    }

}

CTLCSPlugin::~CTLCSPlugin()
{
}

#include "ctl_cs_plugin.moc"
