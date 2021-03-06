#include "divelistmodel.h"
#include "helpers.h"
#include <QDateTime>

DiveListModel *DiveListModel::m_instance = NULL;

DiveListModel::DiveListModel(QObject *parent) : QAbstractListModel(parent)
{
	m_instance = this;
}

void DiveListModel::addDive(dive *d)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_dives.append(new DiveObjectHelper(d));
	endInsertRows();
}

void DiveListModel::updateDive(dive *d)
{
	for (int i = 0; i < m_dives.count(); i++) {
		if (m_dives.at(i)->id() == d->id) {
			DiveObjectHelper *newDive = new DiveObjectHelper(d);
			m_dives.replace(i, newDive);
			break;
		}
	}
}

void DiveListModel::clear()
{
	if (m_dives.count()) {
		beginRemoveRows(QModelIndex(), 0, m_dives.count() - 1);
		qDeleteAll(m_dives);
		m_dives.clear();
		endRemoveRows();
	}
}

int DiveListModel::rowCount(const QModelIndex &) const
{
	return m_dives.count();
}

QVariant DiveListModel::data(const QModelIndex &index, int role) const
{
	if(index.row() < 0 || index.row() > m_dives.count())
		return QVariant();

	DiveObjectHelper *curr_dive = m_dives[index.row()];
	switch(role) {
	case DiveRole: return QVariant::fromValue<QObject*>(curr_dive);
	case DiveDateRole: return (qlonglong)curr_dive->timestamp();
	}
	return QVariant();

}

QHash<int, QByteArray> DiveListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[DiveRole] = "dive";
	roles[DiveDateRole] = "date";
	return roles;
}

// create a new dive. set the current time and add it to the end of the dive list
QString DiveListModel::startAddDive()
{
	struct dive *d;
	d = alloc_dive();
	d->when = QDateTime::currentMSecsSinceEpoch() / 1000L + gettimezoneoffset();
	struct dive *pd = get_dive(dive_table.nr - 1);
	int nr = 1;
	if (pd && pd->number > 0)
		nr = pd->number + 1;
	d->number = nr;
	d->dc.model = strdup("manually added dive");
	add_single_dive(-1, d);
	addDive(d);
	return QString::number(d->id);
}

DiveListModel *DiveListModel::instance()
{
	return m_instance;
}

DiveObjectHelper* DiveListModel::at(int i){
	return m_dives.at(i);
}