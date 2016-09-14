#ifndef GUI_PLANNINGBOARDWIDGET_H
#define GUI_PLANNINGBOARDWIDGET_H

#include "gui/planningwidget/planningboardlayout.h"

#include "hotel/hotel.h"
#include "hotel/hotelcollection.h"
#include "hotel/persistence/sqlitestorage.h"
#include "hotel/planning.h"
#include "hotel/reservation.h"

#include <QGraphicsView>

#include <boost/date_time.hpp>

#include <map>
#include <memory>
#include <string>

namespace gui
{
  namespace planningwidget
  {

    /**
     * @brief The PlanningBoardWidget class is a widget showing the reservations onto a virtual planning board
     *
     * @see PlanningBoardReservationItem
     */
    class PlanningBoardWidget : public QGraphicsView
    {
    public:
      PlanningBoardWidget(const PlanningBoardLayout* layout);
      void addReservations(const std::vector<std::unique_ptr<hotel::Reservation>>& reservations);

      //! When the layout changes, call this methods to update the scene.
      void updateLayout();

    protected:
      virtual void drawBackground(QPainter* painter, const QRectF& rect) override;
      virtual void drawForeground(QPainter* painter, const QRectF& rect) override;

    private:
      hotel::PlanningBoard* _planning;
      hotel::HotelCollection* _hotels;

      QGraphicsScene* _scene;
      const PlanningBoardLayout* _layout;

      void invalidateBackground();
      void invalidateForeground();
    };

  } // namespace planningwidget
} // namespace gui

#endif // GUI_PLANNINGBOARDWIDGET_H
