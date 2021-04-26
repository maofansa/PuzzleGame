#include "mainwindow.h"
#include "puzzle/pieceslist.h"
#include "puzzle/puzzlewidget.h"
#include "bmp/bmp.h"

#include <QtWidgets>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenus();
    setupWidgets();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowTitle(tr("拼图"));
}

void MainWindow::openImage()
{
    const QString directory =
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath());
    QFileDialog dialog(this, tr("Open Image"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QStringList mimeTypeFilters;
    for (const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/bmp");

    QString str_q;
    if (dialog.exec() == QDialog::Accepted)
        //loadImage(dialog.selectedFiles().constFirst());
        str_q = dialog.selectedFiles().first();

    std::string str_s = str_q.toStdString();
    const char* path_c = str_s.c_str();
    Bmp bmp;
    Bmp cutBmp;
    bmp.loadImg(path_c);
    if(bmp.getWidth() > bmp.getHeight())
    {
        int cutLength = bmp.getHeight();
        int cutLeftX = ((bmp.getWidth()-cutLength)%2) ? (bmp.getWidth()-cutLength)/2+1 : (bmp.getWidth()-cutLength)/2;
        bmp.cutImg(cutBmp, cutLeftX, 1, cutLeftX+cutLength, cutLength);
    }
    else
    {
        int cutLength = bmp.getWidth();
        int cutLeftY = ((bmp.getHeight()-cutLength)%2) ? (bmp.getHeight()-cutLength)/2+1 : (bmp.getHeight()-cutLength)/2;
        bmp.cutImg(cutBmp, 1, cutLeftY, cutLength, cutLeftY+cutLength);
    }

    // 绝对路径才可以正常加载其他图片
    char colorImg_c[] = "D:\\Repositories\\PuzzleGame\\images\\colorImg1.bmp"; //":/images/colorImg.bmp";
    char greyImg_c[] =  "D:\\Repositories\\PuzzleGame\\images\\greyImg1.bmp"; //":/images/greyImg.bmp";
    const QString path_q = QString(QLatin1String(colorImg_c));
    const QString grey_path_q = QString(QLatin1String(greyImg_c));

    cutBmp.saveImg(colorImg_c);
    cutBmp.toGray(); cutBmp.saveImg(greyImg_c);
    //bmp.toGray(); bmp.saveImg(colorImg_c);


    loadImage(path_q, grey_path_q);
}

void MainWindow::loadImage(const QString &fileName, const QString &grey_fileName)
{
    QPixmap newImage;
    if (!newImage.load(fileName)) {
        QMessageBox::warning(this, tr("Open Image"),
                             tr("The image file could not be loaded."),
                             QMessageBox::Close);
        return;
    }
    puzzleImage = newImage;

    QPixmap grey_newImage;
    if (!grey_newImage.load(grey_fileName)) {
        QMessageBox::warning(this, tr("Open Grey Image"),
                             tr("The grey image file could not be loaded."),
                             QMessageBox::Close);
        return;
    }
    grey_newImage = grey_newImage.scaled(QSize(100, 100), Qt::KeepAspectRatio);
    greyImage->setPixmap(grey_newImage);

    setupPuzzle();
}

void MainWindow::setCompleted()
{
    QMessageBox::information(this, tr("拼图完成"),
                             tr("恭喜你！你完成了拼图。\n"
                                "点击OK继续游戏。"),
                             QMessageBox::Ok);

    setupPuzzle();
}

void MainWindow::setupPuzzle()
{
    int size = qMin(puzzleImage.width(), puzzleImage.height());
    puzzleImage = puzzleImage.copy((puzzleImage.width() - size) / 2,
        (puzzleImage.height() - size) / 2, size, size).scaled(puzzleWidget->width(),
            puzzleWidget->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    piecesList->clear();

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int pieceSize = puzzleWidget->pieceSize();
            QPixmap pieceImage = puzzleImage.copy(x * pieceSize, y * pieceSize, pieceSize, pieceSize);
            piecesList->addPiece(pieceImage, QPoint(x, y));
        }
    }

    for (int i = 0; i < piecesList->count(); ++i) {
        if (QRandomGenerator::global()->bounded(2) == 1) {
            QListWidgetItem *item = piecesList->takeItem(i);
            piecesList->insertItem(0, item);
        }
    }

    puzzleWidget->clear();
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("文件"));

    QAction *openAction = fileMenu->addAction(tr("打开..."), this, &MainWindow::openImage);
    openAction->setShortcuts(QKeySequence::Open);

    QAction *exitAction = fileMenu->addAction(tr("退出"), qApp, &QCoreApplication::quit);
    exitAction->setShortcuts(QKeySequence::Quit);

    QMenu *gameMenu = menuBar()->addMenu(tr("游戏"));

    gameMenu->addAction(tr("重新开始"), this, &MainWindow::setupPuzzle);
}

void MainWindow::setupWidgets()
{
    QFrame *frame = new QFrame;
    QHBoxLayout *frameLayout = new QHBoxLayout(frame);
    puzzleWidget = new PuzzleWidget(400);

    piecesList = new PiecesList(puzzleWidget->pieceSize(), this);


    connect(puzzleWidget, &PuzzleWidget::puzzleCompleted,
            this, &MainWindow::setCompleted, Qt::QueuedConnection);

    //QLabel *greyImgLB = new QLabel();
    greyImage = new QLabel();
    QPixmap greyPix(":/images/greyImg.bmp");
    greyPix = greyPix.scaled(QSize(100, 100), Qt::KeepAspectRatio);
    greyImage->setPixmap(greyPix);
    //greyImgLB->setPixmap(greyPix);
    //greyImgLB->setScaledContents(true);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(greyImage);
    leftLayout->addWidget(piecesList);
    frameLayout->addLayout(leftLayout);
    //frameLayout->addWidget(piecesList);
    frameLayout->addWidget(puzzleWidget);
    setCentralWidget(frame);
}

