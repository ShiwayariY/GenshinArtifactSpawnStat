#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtGui/QPixmap>
#include <QtGui/QKeyEvent>

#ifndef INCLUDE_GENSHINARTIFACTSPAWNSTAT_INVESTIGATIONENTRY_HH_
#define INCLUDE_GENSHINARTIFACTSPAWNSTAT_INVESTIGATIONENTRY_HH_

namespace GenshinArtifactSpawnStat {

class InvestigationEntry : public QFrame {
	Q_OBJECT

	QPixmap m_map_image;
	QLabel* m_map_label;
	QPixmap m_screenshot_image;
	QLabel* m_screenshot_label;
	QWidget* m_drop_choice_widget;
	QRadioButton* m_1_one_star_button;
	QRadioButton* m_2_one_star_button;
	QRadioButton* m_1_two_star_button;

	void load_images(const QString& map_path, const QString& screenshot_path);
	void init_drop_choice_box();

protected:
	void keyPressEvent(QKeyEvent*) override;

public:
	InvestigationEntry(const QString& map_path, const QString& screenshot_path);

	enum class Drop {
		None,
		SingleOneStar,
		DoubleOneStar,
		SingleTwoStar
	};
	Drop drop() const;
	void zoom(double factor);
	void enable_choice(bool);
};

}

#endif