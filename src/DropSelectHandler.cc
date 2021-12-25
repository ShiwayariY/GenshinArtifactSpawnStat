#include <QtCore/QPoint>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QScrollBar>

#include <DropSelectHandler.hh>

namespace GenshinArtifactSpawnStat {

DropSelectHandler::DropSelectHandler(std::vector<QWidget*> focus_chain, QScrollArea* focus_chain_container) :
		m_focus_chain{ focus_chain },
		m_focus_chain_container{ focus_chain_container } {}

bool DropSelectHandler::check_focus(const QWidget* w) const {
	if (w->hasFocus()) return true;
	for (const auto* child : w->findChildren<QWidget*>())
		if (child->hasFocus()) return true;

	return false;
}

std::size_t DropSelectHandler::focus_index() const {
	std::size_t focus_i = 0;
	while (focus_i < m_focus_chain.size() && !check_focus(m_focus_chain[focus_i]))
		++focus_i;
	return focus_i;
}

bool DropSelectHandler::valid_focus_index(std::size_t i) const {
	return !m_focus_chain.empty() && i < m_focus_chain.size();
}

void DropSelectHandler::focus_next() {
	auto curr = focus_index();
	if (!valid_focus_index(curr)) return;

	if (curr == m_focus_chain.size() - 1) return;
	set_focus(m_focus_chain[curr + 1]);
}

void DropSelectHandler::focus_prev() {
	auto curr = focus_index();
	if (!valid_focus_index(curr)) return;

	if (curr == 0) return;
	set_focus(m_focus_chain[curr - 1]);
}

void DropSelectHandler::set_focus(QWidget* target) {
	target->setFocus();
	auto target_pos = target->mapTo(m_focus_chain_container, QPoint{ 0, 0 });
	auto* vbar = m_focus_chain_container->verticalScrollBar();
	vbar->setValue(vbar->value() + target_pos.y());
}

bool DropSelectHandler::eventFilter(QObject*, QEvent* e) {
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);
		switch (ke->key()) {
			case Qt::Key_1:
			case Qt::Key_2:
			case Qt::Key_3:
				focus_next();
				break;
			case Qt::Key_Up:
				focus_prev();
				return true;
			case Qt::Key_Down:
				focus_next();
				return true;
		}
	}
	return false;
}

}