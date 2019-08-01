#include "DAE.hpp"
#include "assimp/Importer.hpp"

namespace Panda
{
    std::unique_ptr<Scene> DaeParser::Parse(const std::string& file)
    {
        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(file, 0);
		std::unique_ptr<Scene> scene(new Scene("DAE Scene"));
		return scene;
    }
}