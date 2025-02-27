#ifndef DISTRIBUTIONANDREACTIONSMODEL_HPP
#define DISTRIBUTIONANDREACTIONSMODEL_HPP

#include "DiscType.hpp"

#include <QMap>
#include <QObject>
#include <QString>

#include <map>

struct DiscTypeInput
{
    QString oldName;
    QString newName;
    int radius;
    int mass;
    QString color;
    int frequency;
};

struct ReactionInput
{
    int id;
    QString A;
    QString B;
    QString C;
    QString D;
    int probability;
};

class DistributionAndReactionsModel : public QObject
{
    Q_OBJECT
public:
    void updateDiscType(const DiscTypeInput& discTypeInput);
    void removeDiscType(const QString& name);

    std::map<DiscType, int> getDiscTypeDistribution() const;

    void updateReaction(const ReactionInput& reactionInput);
    void removeReaction(int id);

    std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>
    getCombinationReactionsTable() const;
    std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>
    getDecompositionReactionsTable() const;

private:
};

#endif /* DISTRIBUTIONANDREACTIONSMODEL_HPP */
