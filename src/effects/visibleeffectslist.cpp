#include "effects/visibleeffectslist.h"

#include <QDomDocument>

#include "effects/backends/effectsbackendmanager.h"
#include "effects/presets/effectxmlelements.h"
#include "util/xml.h"

void VisibleEffectsList::setList(const QList<EffectManifestPointer>& newList) {
    m_list = newList;
    emit visibleEffectsListChanged();
}

void VisibleEffectsList::readEffectsXml(
        const QDomDocument& doc, EffectsBackendManagerPointer pBackendManager) {
    QDomElement root = doc.documentElement();
    QDomElement visibleEffectsElement = XmlParse::selectElement(root, EffectXml::VisibleEffects);
    QDomNodeList effectsElementsList = visibleEffectsElement.elementsByTagName(EffectXml::Effect);
    QList<EffectManifestPointer> list;

    for (int i = 0; i < effectsElementsList.count(); ++i) {
        QDomNode effectNode = effectsElementsList.at(i);
        if (effectNode.isElement()) {
            QString id = XmlParse::selectNodeQString(effectNode, EffectXml::EffectId);
            QString backendString = XmlParse::selectNodeQString(
                    effectNode, EffectXml::EffectBackendType);
            EffectBackendType backendType = EffectsBackend::backendTypeFromString(backendString);
            EffectManifestPointer pManifest = pBackendManager->getManifest(id, backendType);
            if (pManifest) {
                list.append(pManifest);
            }
        }
    }

    if (!list.isEmpty()) {
        setList(list);
    } else {
        setList(pBackendManager->getManifestsForBackend(EffectBackendType::BuiltIn));
    }
}

void VisibleEffectsList::saveEffectsXml(QDomDocument* pDoc) {
    QDomElement root = pDoc->documentElement();
    QDomElement visibleEffectsElement = pDoc->createElement(EffectXml::VisibleEffects);
    root.appendChild(visibleEffectsElement);
    for (const auto& pManifest : m_list) {
        VERIFY_OR_DEBUG_ASSERT(pManifest) {
            continue;
        }
        QDomElement effectElement = pDoc->createElement(EffectXml::Effect);
        visibleEffectsElement.appendChild(effectElement);
        XmlParse::addElement(*pDoc, effectElement, EffectXml::EffectId, pManifest->id());
        XmlParse::addElement(*pDoc,
                effectElement,
                EffectXml::EffectBackendType,
                EffectsBackend::backendTypeToString(pManifest->backendType()));
    }
}
