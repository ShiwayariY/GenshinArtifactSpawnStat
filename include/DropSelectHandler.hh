#include <vector>

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollArea>

#ifndef INCLUDE_GENSHINARTIFACTSPAWNSTAT_DROPSELECTHANDLER_HH_
#define INCLUDE_GENSHINARTIFACTSPAWNSTAT_DROPSELECTHANDLER_HH_

namespace GenshinArtifactSpawnStat {

class DropSelectHandler : public QObject {
	Q_OBJECT

	std::vector<QWidget*> m_focus_chain;
	QScrollArea* m_focus_chain_container;

	bool check_focus(const QWidget*) const;
	std::size_t focus_index() const;
	bool valid_focus_index(std::size_t) const;
	void focus_next();
	void focus_prev();
	void set_focus(QWidget*);

public:
	DropSelectHandler(std::vector<QWidget*> focus_chain, QScrollArea* focus_chain_container);
	bool eventFilter(QObject*, QEvent*) override;
};

}

#endif