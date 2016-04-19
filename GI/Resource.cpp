#include "JobFactory.h"
#include "Resource.h"
#include "Renderer.h"
#include "Engine.h"
#include "Scheduler.h"
#include "Program.h"
#include "Shader.h"

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

bool nxResourceLooper::operator()(void* data) {
    nxResourceLooperBlob* blob = (nxResourceLooperBlob*)data;

    if (blob->m_Engine->IsRendererFinished())
        return false;

    if (blob->m_Engine->IsSchedulerFinished())
        return false;

    //std::cout << "Checking resources " << std::endl;
    for (auto& l_ProgramPair : blob->m_Renderer->GetPrograms()) {
        auto l_Program = l_ProgramPair.second;

        if (l_Program == nullptr)
            continue;

        //std::cout << "Program name : " << l_Program->GetName() << std::endl;
        auto l_ShaderIndex = 0;
        for (auto l_Shader : l_Program->GetShaders()) {
            //std::cout << "Shader path : " << l_Shader->Filename() << std::endl;
            std::time_t l_Tick = boost::posix_time::to_time_t(boost::posix_time::second_clock::local_time());
            std::time_t l_WriteTime = boost::filesystem::last_write_time(l_Shader->Filename());
            std::time_t l_LastMod = l_Shader->LastMod();
            //std::cout << "UTC Now: " << std::asctime(std::gmtime(&l_Tick));
            //std::cout << "UTC Write: " << std::asctime(std::gmtime(&l_WriteTime));
            //std::cout << "UTC Last: " << std::asctime(std::gmtime(&l_LastMod));
            if (l_WriteTime > l_LastMod) {
                // Schedule Reload
                l_Shader->SetLastMod(l_WriteTime);
                nxShaderLoaderBlob* data = new nxShaderLoaderBlob(blob->m_Engine, l_Program, l_Shader->Filename(), l_Shader->ShaderType(), l_ShaderIndex, false);
                blob->m_Engine->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_RELOAD_SHADER, data));
            }

            l_ShaderIndex++;
        }
    }

    blob->m_Engine->Scheduler()->ScheduleOwnJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_MANAGE_RESOURCES, blob, true, 2000000000));

    return true;
};