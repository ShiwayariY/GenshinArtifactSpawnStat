#include <filesystem>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QImageReader>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include <InvestigationEntry.hh>

namespace GenshinArtifactSpawnStat {

InvestigationEntry::InvestigationEntry(const QString& map_path, const QString& screenshot_path) {
	setFrameShape(QFrame::Panel);

	load_images(map_path, screenshot_path);
	init_drop_choice_box();

	QHBoxLayout* entry_layout = new QHBoxLayout;
	entry_layout->addWidget(m_map_label);
	entry_layout->addWidget(m_screenshot_label);
	entry_layout->addWidget(m_drop_choice_widget, 0, Qt::AlignRight);

	setLayout(entry_layout);
	setFocusPolicy(Qt::StrongFocus);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setAutoFillBackground(true);
	adjustSize();
}

void InvestigationEntry::load_images(const QString& map_path, const QString& screenshot_path) {
	auto create_black_img = []() {
		QPixmap img(400, 400);
		QPainter p(&img);
		img.fill(QColor::fromRgb(0, 0, 0));
		return img;
	};

	m_map_label = new QLabel;
	m_map_image = create_black_img();
	if (std::filesystem::is_regular_file(map_path.toStdString())) {
		QImageReader map_reader{ map_path };
		if (map_reader.canRead())
			m_map_image = QPixmap::fromImageReader(&map_reader)
							.scaled(1000, 400, Qt::AspectRatioMode::KeepAspectRatio);
	}
	m_map_label->setPixmap(m_map_image);
	m_map_label->setFixedSize(m_map_image.size());

	m_screenshot_label = new QLabel;
	m_screenshot_image = create_black_img();
	if (std::filesystem::is_regular_file(screenshot_path.toStdString())) {
		QImageReader screenshot_reader{ screenshot_path };
		if (screenshot_reader.canRead())
			m_screenshot_image = QPixmap::fromImageReader(&screenshot_reader)
								   .scaled(1000, 400, Qt::AspectRatioMode::KeepAspectRatio);
	}
	m_screenshot_label->setPixmap(m_screenshot_image);
	m_screenshot_label->setFixedSize(m_screenshot_image.size());
}

void InvestigationEntry::init_drop_choice_box() {
	m_1_one_star_button = new QRadioButton{ "★" };
	m_2_one_star_button = new QRadioButton{ "★ x2" };
	m_1_two_star_button = new QRadioButton{ "★★" };

	QVBoxLayout* drop_choice_layout = new QVBoxLayout;
	drop_choice_layout->addWidget(m_1_one_star_button);
	drop_choice_layout->addWidget(m_2_one_star_button);
	drop_choice_layout->addWidget(m_1_two_star_button);

	m_drop_choice_widget = new QWidget;
	m_drop_choice_widget->setStyleSheet("background-color: rbga(0,0,0,0)");
	m_drop_choice_widget->setLayout(drop_choice_layout);
}

void InvestigationEntry::keyPressEvent(QKeyEvent* e) {
	if (!e->isAutoRepeat()) {
		if (e->key() == Qt::Key_1) m_1_one_star_button->toggle();
		if (e->key() == Qt::Key_2) m_2_one_star_button->toggle();
		if (e->key() == Qt::Key_3) m_1_two_star_button->toggle();
	}
	QWidget::keyPressEvent(e);
}

InvestigationEntry::Drop InvestigationEntry::drop() const {
	if (m_1_one_star_button->isChecked()) return Drop::SingleOneStar;
	if (m_2_one_star_button->isChecked()) return Drop::DoubleOneStar;
	if (m_1_two_star_button->isChecked()) return Drop::SingleTwoStar;
	return Drop::None;
}

void InvestigationEntry::zoom(double factor) {
	m_map_image = m_map_image.scaled(m_map_image.size() * factor);
	m_map_label->setPixmap(m_map_image);
	m_map_label->setFixedSize(m_map_image.size());

	m_screenshot_image = m_screenshot_image.scaled(m_screenshot_image.size() * factor);
	m_screenshot_label->setPixmap(m_screenshot_image);
	m_screenshot_label->setFixedSize(m_screenshot_image.size());

	resize(size().width(), 500);
}

void InvestigationEntry::enable_choice(bool enable) {
	m_1_one_star_button->setEnabled(enable);
	m_2_one_star_button->setEnabled(enable);
	m_1_two_star_button->setEnabled(enable);
}

}